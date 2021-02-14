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

#include <unistd.h>
#include <stdlib.h>
#include <termio.h>
#include <pty.h>
#include <glib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "session.h"

#define D_DM(s)
#define D_DIE(s)

static SSH_SESSION *session_free (SSH_SESSION *the_session) ;
static SSH_SESSION *session_new (char *key) ;
static gboolean compare_keys (gconstpointer k1, gconstpointer k2) ;
static int create_child (char *, char **, int, int, pid_t *) ;
static gpointer wait_for_ssh_to_die_thread (gpointer p) ;

static GStaticMutex global_session_hash_table_mutex = G_STATIC_MUTEX_INIT ;
static GHashTable *global_session_hash_table = NULL ;

void session_init ()
  {global_session_hash_table = g_hash_table_new_full (g_str_hash, compare_keys, NULL, (GDestroyNotify)session_free) ;}

void session_uninit ()
  {
  g_static_mutex_lock (&global_session_hash_table_mutex) ;
  g_hash_table_destroy (global_session_hash_table) ;
  global_session_hash_table = NULL ;
  g_static_mutex_unlock (&global_session_hash_table_mutex) ;
  }

static gboolean compare_keys (gconstpointer k1, gconstpointer k2)
  {return !strcmp ((char *)k1, (char *)k2) ;}

SSH_SESSION *session_get (char *key)
  {
  SSH_SESSION *the_session = NULL ;

  if (NULL == key) return NULL ;

  g_static_mutex_lock (&global_session_hash_table_mutex) ;

  if (NULL == (the_session = g_hash_table_lookup (global_session_hash_table, key)))
    if (NULL != (the_session = session_new (key)))
      g_hash_table_insert (global_session_hash_table, the_session->key, the_session) ;

  if (!g_mutex_trylock (the_session->delete_mutex))
    // If somebody already has this session, noone else may have it
    the_session = NULL ;
  else
  if (the_session->is_dead)
    {
    g_hash_table_remove (global_session_hash_table, the_session->key) ;
    the_session = NULL ;
    }

  g_static_mutex_unlock (&global_session_hash_table_mutex) ;

  return the_session ;
  }

void session_return (SSH_SESSION *the_session)
  {
  if (NULL == the_session) return ;

  D_DM (g_print ("session_return: Unlocking delete_mutex\n")) ;
  g_mutex_unlock (the_session->delete_mutex) ;
  }

void session_remove (SSH_SESSION *the_session)
  {
  if (NULL == the_session) return ;

  g_static_mutex_lock (&global_session_hash_table_mutex) ;
  g_hash_table_remove (global_session_hash_table, the_session->key) ;
  g_static_mutex_unlock (&global_session_hash_table_mutex) ;
  }

static SSH_SESSION *session_new (char *key)
  {
  SSH_SESSION *the_session = g_malloc0 (sizeof (SSH_SESSION)) ;

  if (NULL != the_session)
    {
    if (NULL == (the_session->key = g_strdup (key)))
      {
      g_free (the_session) ;
      return NULL ;
      }

    if (NULL == (the_session->delete_mutex = g_mutex_new ()))
      {
      g_free (the_session->key) ;
      g_free (the_session) ;
      return NULL ;
      }

    the_session->ssh_pid       =  0 ;
    the_session->ssh_fd        = -1 ;
    the_session->screen_buffer = NULL ;
    the_session->is_dead       = FALSE ;
    }
  return the_session ;
  }

// This function always returns NULL
static SSH_SESSION *session_free (SSH_SESSION *the_session)
  {
  GMutex *the_mutex = NULL ;

  if (NULL == the_session) return NULL ;

  g_mutex_trylock (the_mutex = the_session->delete_mutex) ;

  if (NULL != the_session->key)
    g_free (the_session->key) ;
  screen_buffer_free (the_session->screen_buffer) ;
  g_free (the_session) ;

  g_mutex_unlock (the_mutex) ;
  g_mutex_free (the_mutex) ;

  return NULL ;
  }

gboolean session_spawn (SSH_SESSION *the_session, char *hostname, char *username, int rows, int cols)
  {
  char *ssh_argv[] = {"ssh", "-e", "none", "-q", "-l", username, hostname, NULL} ;
  //char *ssh_argv[] = {"links", "google.com/ncr", NULL} ;

  if (NULL == the_session || NULL == hostname || NULL == username) ;

  the_session->ssh_fd = create_child ("/usr/bin/ssh", ssh_argv, rows, cols, &(the_session->ssh_pid)) ;

  if (-1 == the_session->ssh_fd) return FALSE ;

  if (NULL == (the_session->screen_buffer = screen_buffer_new (CLAMP (rows, MIN_ROWS, MAX_ROWS), CLAMP (cols, MIN_COLS, MAX_COLS))))
    {
    int status ;
    kill (the_session->ssh_pid, SIGKILL) ;
    waitpid (the_session->ssh_pid, &status, 0) ;
    return FALSE ;
    }
  g_thread_create (wait_for_ssh_to_die_thread, the_session, FALSE, NULL) ;

  return TRUE ;
  }

static int create_child (char *command, char **argv, int rows, int cols, pid_t *p_pid)
  {
  pid_t child_pid ;
  int master_fd = -1 ;
  struct winsize ws = {rows, cols} ;

  if (-1 == (child_pid = forkpty (&master_fd, NULL, NULL, &ws)))
    {
    if (-1 != master_fd)
      close (master_fd) ;
    master_fd = -1 ;
    }
  else
  if (child_pid > 0)
    (*p_pid) = child_pid ;
  else
    {
    unsetenv ("DISPLAY") ;
    setenv ("TERM", "xterm", 1) ;
    execvp (command, argv) ;
    }

  return master_fd ;
  }

/*
static int create_child (char *command, char **argv, int rows, int cols, pid_t *p_pid)
  {
  int master_fd, slave_fd, Nix ;
  struct winsize ws = {0} ;

struct termios ttmode = 
  {
// ICRNL |
  BRKINT | IGNPAR | IXON | IMAXBEL,
//  OCRNL |
  OPOST | NL0 | CR0 | TAB0 | BS0 | VT0 | FF0,
  CS8 | CREAD,
  ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE,
  0,
      { 003, 034, 0177, 025,
        004, 000, 001, 000,
        021, 023, 032, 022,
        022, 017, 027, 026
      }
  } ;
        
    ws.ws_row = rows ;
    ws.ws_col = cols ;
    
  cfsetospeed (&ttmode, B38400) ;
  cfsetispeed (&ttmode, B38400) ;
    
  openpty (&master_fd, &slave_fd, 0, &ttmode, &ws) ;
  if (-1 == master_fd || -1 == slave_fd) return -1 ;
  if (!((*p_pid) = fork ()))
    {
    ioctl (0, TIOCNOTTY) ;
    setsid () ;
    // Close all file descriptors except the tty fd
    for (Nix = 0 ; Nix < getdtablesize () ; Nix++)
        if (slave_fd != Nix)
            close (Nix) ;
    dup (slave_fd) ;
    dup (slave_fd) ;
    dup (slave_fd) ;

    ioctl (slave_fd, TIOCSCTTY, slave_fd) ;
    unsetenv ("DISPLAY") ;
    setenv ("TERM", "xterm", 1) ;
    execvp (command, argv) ;
    return -1 ;
    }
  if (-1 == (*p_pid))
    {
    close (master_fd) ;
    close (slave_fd) ;
    return -1 ;
    }
  return master_fd ;
  }
*/
static gpointer wait_for_ssh_to_die_thread (gpointer p)
  {
  int status ;

  SSH_SESSION *the_session = (SSH_SESSION *)p ;

  waitpid (the_session->ssh_pid, &status, 0) ;
  the_session->is_dead = TRUE ;
  D_DIE (g_print ("wait_for_ssh_to_die_thread: ssh has died\n") ;)

  return NULL ;
  }
