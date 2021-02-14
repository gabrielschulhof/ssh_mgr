// This file is part of ssh_mgr2
// Copyright (C) 2006 Gabriel Schulhof <nix@go-nix.ca>
// Package ssh_mgr2 released under GPL (http://www.gnu.org/licenses/gpl.txt):
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <glib.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include "client.h"
#include "session.h"
#include "screen_buffer.h"
#include "io.h"

#define D_TRACE(s) s
#define D_DRAINS(s)

static gboolean drain_socket (int fd, EXP_ARRAY *bucket) ;
static gboolean drain_socket_iter (int fd, double timeout, EXP_ARRAY *bucket) ;
static gboolean spawn_session (SSH_SESSION *the_session, IO_WRAPPER *io_wrapper) ;

gpointer client_thread (gpointer data)
  {
  char *key = NULL ;
  SSH_SESSION *the_session = NULL ;
  int client_socket = GPOINTER_TO_INT (data) ;
  IO_WRAPPER *io_wrapper = NULL ;
  EXP_ARRAY *bucket = NULL ;

  D_TRACE (g_print ("client_thread: Entering\n")) ;

  if (NULL == (io_wrapper = io_wrap_fd (client_socket)))
    {
    D_TRACE (g_print ("client_thread: io_wrap_fd failed\n")) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    close (client_socket) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: io_wrap_fd suceeded\n")) ;

  if (NULL == (key = io_wrapper_read_tag (io_wrapper, "key")))
    {
    D_TRACE (g_print ("client_thread: io_wrapper_read_tag: \"key\" failed\n")) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: io_wrapper_read_tag: \"key\" suceeded: \"%s\"\n", key)) ;

  the_session = session_get (key) ;
  g_free (key) ;
  if (NULL == the_session)
    {
    D_TRACE (g_print ("client_thread: session_get: failed\n")) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: session_get: succeeded: 0x%x\n", (int)the_session)) ;

  if (0 == the_session->ssh_pid)
    {
    D_TRACE (g_print ("client_thread: It's a new session\n")) ;
    write (client_socket, "<status>New</status>\n", 21) ;
    if (!spawn_session (the_session, io_wrapper))
      {
      D_TRACE (g_print ("client_thread: spawn_session: failed\n")) ;
      session_remove (the_session) ;
      write (client_socket, "<status>Dead</status>\n", 22) ;
      io_wrapper = io_wrapper_close_fd (io_wrapper) ;
      return NULL ;
      }
    D_TRACE (else g_print ("client_thread: spawn_session: succeeded\n")) ;
    }
  else
    {
    D_TRACE (g_print ("It's an old session\n")) ;
    write (client_socket, "<status>Old</status>\n", 21) ;
    }

  // Looking good so far - the ssh process could still be dead, though

  if (NULL == (bucket = io_wrapper_read_keystrokes (io_wrapper)))
    {
    D_TRACE (g_print ("client_thread: io_wrapper_read_keystrokes: failed\n")) ;
    kill (the_session->ssh_pid, SIGKILL) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }

  if (write (the_session->ssh_fd, &exp_array_index_1d (bucket, char, 0), bucket->icUsed) < 0)
    {
    D_TRACE (g_print ("client_thread: write: Failed to write to ssh\n")) ;
    exp_array_free (bucket) ;
    kill (the_session->ssh_pid, SIGKILL) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: write: Succeeded writing to ssh\n")) ;

  // Having poured the data from the PHP socket to the ssh socket, clean the bucket and
  // prepare it for reading ssh's reaction
  exp_array_remove_vals (bucket, 1, 0, bucket->icUsed) ;

  if (!drain_socket (the_session->ssh_fd, bucket))
    {
    D_TRACE (g_print ("client_thread: drain_socket: failed\n")) ;
    exp_array_free (bucket) ;
    kill (the_session->ssh_pid, SIGKILL) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: drain_socket: succeeded\n")) ;

  screen_buffer_interpret (the_session->screen_buffer, bucket) ;
  if (!screen_buffer_write_to_socket (the_session->screen_buffer, client_socket))
    {
    D_TRACE (g_print ("client_thread: screen_buffer_write_to_socket: failed\n")) ;
    exp_array_free (bucket) ;
    kill (the_session->ssh_pid, SIGKILL) ;
    write (client_socket, "<status>Dead</status>\n", 22) ;
    io_wrapper = io_wrapper_close_fd (io_wrapper) ;
    return NULL ;
    }
  D_TRACE (else g_print ("client_thread: screen_buffer_write_to_socket: succeeded\n")) ;

  exp_array_free (bucket) ;
  session_return (the_session) ;
  write (client_socket, "<status>Alive</status>\n", 23) ;

  D_TRACE (g_print ("client_thread: Exiting\n\n")) ;
  io_wrapper = io_wrapper_close_fd (io_wrapper) ;

  return NULL ;
  }

static gboolean drain_socket (int fd, EXP_ARRAY *bucket)
  {
  gdouble timer_remaining ;
  GTimer *timer = NULL ;

  D_TRACE (g_print ("drain_socket: Entering\n")) ;

  // Independently of the above timer, wait GRAB_KEYSTROKE_INIT_TIME for initial output
  drain_socket_iter (fd, GRAB_KEYSTROKE_INIT_TIME, bucket) ;

  // Then, process output for an additional GRAB_KEYSTROKE_TIME
  if (NULL != (timer = g_timer_new ()))
    {
    while ((timer_remaining = g_timer_elapsed (timer, NULL)) < GRAB_KEYSTROKE_TIME)
      if (!drain_socket_iter (fd, GRAB_KEYSTROKE_TIME - timer_remaining, bucket))
        {
        g_timer_destroy (timer) ;
        return FALSE ;
        }

    g_timer_destroy (timer) ;
    }

  D_TRACE (g_print ("drain_socket: Exiting\n")) ;
  return TRUE ;
  }

static gboolean drain_socket_iter (int fd, double timeout, EXP_ARRAY *bucket)
  {
  fd_set fds ;
  struct timeval tv ;
  guint32 bytes_available = 0, bytes_read = 0, idx = -1 ;

  FD_ZERO (&fds) ;
  FD_SET (fd, &fds) ;

  tv.tv_sec = (int)timeout ;
  timeout = (timeout - (double)tv.tv_sec) * 1e6 ;
  tv.tv_usec = (int)timeout ;

  if (select (fd + 1, &fds, NULL, NULL, &tv) >= 0)
    {
    if (FD_ISSET (fd, &fds))
      {
      bytes_available = 0 ;
      ioctl (fd, FIONREAD, &bytes_available) ;
      if (bytes_available > 0)
        {
        idx = bucket->icUsed ;
        exp_array_1d_insert_vals (bucket, NULL, bytes_available, -1) ;
        if ((bytes_read = read (fd, &exp_array_index_1d (bucket, char, idx), bytes_available)) < 0)
          return FALSE ;
        if (bytes_read < bytes_available)
          exp_array_remove_vals (bucket, 1, idx + bytes_read, bytes_available - bytes_read) ;
        }
      }
    }
  else
    D_TRACE({)
    D_TRACE(g_print ("drain_socket_iter: select() failed\n") ;)
    return FALSE ;
    D_TRACE(})

  return TRUE ;
  }

static gboolean spawn_session (SSH_SESSION *the_session, IO_WRAPPER *io_wrapper)
  {
  int rows = 25, cols = 80 ;
  char *username = NULL, *hostname = NULL, *rows_str = NULL, *cols_str = NULL ;

  if (NULL == (username = io_wrapper_read_tag (io_wrapper, "user_name"))) return FALSE ;

  D_TRACE (g_print ("spawn_session: io_wrapper_read_tag: user_name: \"%s\"\n", username)) ;

  if (NULL == (hostname = io_wrapper_read_tag (io_wrapper, "host_name")))
    {
    g_free (username) ;
    return FALSE ;
    }

  D_TRACE (g_print ("spawn_session: io_wrapper_read_tag: host_name: \"%s\"\n", hostname)) ;

  if (NULL == (rows_str    = io_wrapper_read_tag (io_wrapper, "rows")))
    {
    g_free (username) ;
    g_free (hostname) ;
    return FALSE ;
    }

  D_TRACE (g_print ("spawn_session: io_wrapper_read_tag: rows: \"%s\"\n", rows_str)) ;

  if (NULL == (cols_str    = io_wrapper_read_tag (io_wrapper, "cols")))
    {
    g_free (username) ;
    g_free (hostname) ;
    g_free (rows_str) ;
    return FALSE ;
    }

  D_TRACE (g_print ("spawn_session: io_wrapper_read_tag: cols: \"%s\"\n", cols_str)) ;

  rows = atoi (rows_str) ; g_free (rows_str) ;
  cols = atoi (cols_str) ; g_free (cols_str) ;

  D_TRACE (g_print ("spawn_session: screen buffer size is [rxc] = [%dx%d]\n", rows, cols)) ;

  return session_spawn (the_session, hostname, username, rows, cols) ;
  }
