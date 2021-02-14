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

#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>
#include <glib.h>
#include "exp_array.h"
#include "io.h"

// Gee, I wonder why I use /these/ delimiters :o)
#define TAG_BEG '<'
#define TAG_DIE '/'
#define TAG_END '>'

#define D_IO(s)
#define D_IO_KS(s)

struct _IO_WRAPPER
  {
  int fd ;
  GTimer *timer ;
  EXP_ARRAY *buffer ;
  } ;

static gboolean grab_more_buffer (IO_WRAPPER *io_wrapper) ;
static int find_delimiter (IO_WRAPPER *io_wrapper, char delim, int *iter) ;

IO_WRAPPER *io_wrap_fd (int fd)
  {
  IO_WRAPPER *io_wrapper = NULL ;

  if (fd < 0) return NULL ;
  if (NULL == (io_wrapper = g_malloc0 (sizeof (IO_WRAPPER)))) return NULL ;
  if (NULL == (io_wrapper->buffer = exp_array_new (sizeof (char), 1)))
    {
    g_free (io_wrapper) ;
    return NULL ;
    }
  if (NULL == (io_wrapper->timer = g_timer_new ()))
    {
    exp_array_free (io_wrapper->buffer) ;
    g_free (io_wrapper) ;
    return NULL ;
    }
  io_wrapper->fd = fd ;

  return io_wrapper ;
  }

// This function always returns NULL
IO_WRAPPER *io_wrapper_close_fd (IO_WRAPPER *io_wrapper)
  {
  if (NULL == io_wrapper) return NULL ;

  if (NULL != io_wrapper->buffer)
    exp_array_free (io_wrapper->buffer) ;
  if (-1 != io_wrapper->fd)
    close (io_wrapper->fd) ;
  if (NULL != io_wrapper->timer)
    g_timer_destroy (io_wrapper->timer) ;

  g_free (io_wrapper) ;

  return NULL ;
  }

char *io_wrapper_read_tag (IO_WRAPPER *io_wrapper, char *tag)
  {
  char *tag_data = NULL ;
  int beg_tag_beg_idx = -1, beg_tag_end_idx = -1,
      end_tag_beg_idx = -1, end_tag_end_idx = -1 ;
  int itr = 0, strlen_tag = 0, strlen_data = -1 ;

  if (NULL == io_wrapper || NULL == tag) return NULL ;

  D_IO ({
    char *str = g_strndup (io_wrapper->buffer->data, io_wrapper->buffer->icUsed) ;
    g_print ("io_wrapper_read_tag: Entering with buffer |%s| and tag %s\n", str, tag) ;
    g_free (str) ;
  })

  strlen_tag = strlen (tag) ;

  // Find open tag
  while (TRUE)
    {
    if (-1 == (beg_tag_beg_idx = find_delimiter (io_wrapper, TAG_BEG, &itr))) return NULL ;
    if (-1 == (beg_tag_end_idx = find_delimiter (io_wrapper, TAG_END, &itr))) return NULL ;

    D_IO ({
      char *str = g_strndup (io_wrapper->buffer->data, io_wrapper->buffer->icUsed) ;
      g_print ("io_wrapper_read_tag: Opening tag in buffer |%s| lies between %d and %d\n", str, beg_tag_beg_idx, beg_tag_end_idx) ;
      g_free (str) ;
    })

    if (beg_tag_beg_idx + 1 == beg_tag_end_idx)
      g_warning ("Empty tag in client socket !\n") ;
    else
    if (beg_tag_end_idx - beg_tag_beg_idx - 1 == strlen_tag)
      {
      if (!strncmp (&exp_array_index_1d (io_wrapper->buffer, char, beg_tag_beg_idx + 1), tag, strlen_tag))
        break ;
      }
    }

  // Find close tag
  while (TRUE)
    {
    if (-1 == (end_tag_beg_idx = find_delimiter (io_wrapper, TAG_BEG, &itr))) return NULL ;

    if (end_tag_beg_idx == io_wrapper->buffer->icUsed - 1)
      if (!grab_more_buffer (io_wrapper))
        return NULL ;
    if (exp_array_index_1d (io_wrapper->buffer, char, end_tag_beg_idx + 1) != TAG_DIE)
      {
      g_warning ("Malformed end tag ?\n") ;
      itr++ ;
      continue ;
      }
    else
      {
      end_tag_beg_idx++ ;
      itr++ ;
      }

    if (-1 == (end_tag_end_idx = find_delimiter (io_wrapper, TAG_END, &itr))) return NULL ;

    D_IO ({
      char *str = g_strndup (io_wrapper->buffer->data, io_wrapper->buffer->icUsed) ;
      g_print ("io_wrapper_read_tag: Closing tag in buffer |%s| lies between %d and %d\n", str, end_tag_beg_idx, end_tag_end_idx) ;
      g_free (str) ;
    })

    if (end_tag_beg_idx + 1 == end_tag_end_idx)
      g_warning ("Empty tag in client socket !\n") ;
    else
    if (end_tag_end_idx - end_tag_beg_idx - 1 == strlen_tag)
      {
      if (!strncmp (&exp_array_index_1d (io_wrapper->buffer, char, end_tag_beg_idx + 1), tag, strlen_tag))
        break ;
      }
    }

  strlen_data = end_tag_beg_idx - beg_tag_end_idx - 2 ;

  D_IO (g_print ("io_wrapper_read_tag: strlen_data = %d\n", strlen_data)) ;

  tag_data = g_strndup (&exp_array_index_1d (io_wrapper->buffer, char, beg_tag_end_idx + 1), strlen_data) ;

  exp_array_remove_vals (io_wrapper->buffer, 1, 0, end_tag_end_idx + 1) ;

  D_IO ({
    char *str = g_strndup (io_wrapper->buffer->data, io_wrapper->buffer->icUsed) ;
    g_print ("io_wrapper_read_tag: Remaining buffer: |%s|\n", str) ;
    g_free (str) ;
  })

  return tag_data ;
  }

EXP_ARRAY *io_wrapper_read_keystrokes (IO_WRAPPER *io_wrapper)
  {
  char *length_str = NULL ;
  EXP_ARRAY *ar = NULL ;
  guint32 length = 0 ;

  if (NULL == io_wrapper) return NULL ;

  if (NULL == (length_str = io_wrapper_read_tag (io_wrapper, "input_length"))) return NULL ;

  length = atoi (length_str) ;
  g_free (length_str) ;
  D_IO_KS (g_print ("io_wrapper_read_keystrokes: length = %d\n", length) ;)

  ar = exp_array_new (sizeof (char), 1) ;

  while (io_wrapper->buffer->icUsed < length)
    D_IO_KS({)
    D_IO_KS(g_print ("io_wrapper_read_keystrokes: Grabbing more buffer\n") ;)
    grab_more_buffer (io_wrapper) ;
    D_IO_KS(g_print ("io_wrapper_read_keystrokes: Grabbed more buffer - now have %d\n", io_wrapper->buffer->icUsed) ;)
    D_IO_KS(})

  exp_array_1d_insert_vals (ar, &exp_array_index_1d (io_wrapper->buffer, char, 0), length, 0) ;
  exp_array_remove_vals (io_wrapper->buffer, 1, 0, length) ;

  return ar ;
  }

static int find_delimiter (IO_WRAPPER *io_wrapper, char delim, int *iter)
  {
  int delim_idx = -1 ;

  D_IO (g_print ("find_delimiter: Entering with delim = %c and (*iter) = %d\n", delim, (*iter))) ;

  while (-1 == delim_idx)
    {
    // Fly to the nearest delimiter
    for ( ; (*iter) < io_wrapper->buffer->icUsed ; (*iter)++)
      if (delim == exp_array_index_1d (io_wrapper->buffer, char, (*iter)))
        break ;

    // Got to the end of the buffer without finding the delimiter
    if ((*iter) == io_wrapper->buffer->icUsed)
      {
      // Can't find the delimiter && can't grab more buffer => can't read tag *shrug*
      if (!grab_more_buffer (io_wrapper))
        break ;
      }
    else
      {
      delim_idx = (*iter) ;
      break ;
      }
    }

  D_IO (g_print ("find_delimiter: Exiting with delim_idx = %d\n", delim_idx)) ;

  return delim_idx ;
  }

static gboolean grab_more_buffer (IO_WRAPPER *io_wrapper)
  {
  char c ;
  int bytes_available = 0, idx = -1, bytes_read = -1, bytes_to_delete = 0 ;

  D_IO (g_print ("grab_more_buffer: Entering\n")) ;

  if (ioctl (io_wrapper->fd, FIONREAD, &bytes_available) < 0) return FALSE ;

  D_IO (g_print ("grab_more_buffer: ioctl says %d more bytes available\n", bytes_available)) ;

  // First block until /something/ is available
  if (0 == bytes_available)
    {
    D_IO (g_print ("grab_more_buffer: blocking until /something/ becomes available\n")) ;
    if (read (io_wrapper->fd, &c, 1) <= 0) return FALSE ;
    D_IO (g_print ("grab_more_buffer: something became available: %c\n", c)) ;
    exp_array_1d_insert_vals (io_wrapper->buffer, &c, 1, -1) ;
    }

  if (ioctl (io_wrapper->fd, FIONREAD, &bytes_available) < 0) return FALSE ;

  D_IO (g_print ("grab_more_buffer: after blocking: ioctl says %d more bytes available\n", bytes_available)) ;

  // Then, if more is available, read it in
  if (bytes_available > 0)
    {
    idx = io_wrapper->buffer->icUsed ;
    exp_array_1d_insert_vals (io_wrapper->buffer, NULL, bytes_available, -1) ;

    if ((bytes_read = read (io_wrapper->fd, &exp_array_index_1d (io_wrapper->buffer, char, idx), bytes_available)) < 0) return FALSE ;
    D_IO (g_print ("grab_more_buffer: read in %d more bytes\n", bytes_read)) ;

    if (bytes_read < bytes_available)
      {
      bytes_to_delete = bytes_available - bytes_read ;
      D_IO (g_print ("grab_more_buffer: deleting %d end-most bytes from buffer\n", bytes_to_delete)) ;
      exp_array_remove_vals (io_wrapper->buffer, 1, io_wrapper->buffer->icUsed - bytes_to_delete, bytes_to_delete) ;
      }
    }

  // Having read at least one byte, indicate success
  return TRUE ;
  }
