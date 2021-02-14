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

#ifndef _SESSION_H_
#define _SESSION_H_

#include <glib.h>
#include "screen_buffer.h"

G_BEGIN_DECLS

typedef struct _SSH_SESSION SSH_SESSION ;

struct _SSH_SESSION
  {
  GMutex *delete_mutex ;
  char *key ;
  pid_t ssh_pid ;
  int ssh_fd ;
  SCREEN_BUFFER *screen_buffer ;
  gboolean is_dead ;
  } ;

void session_init () ;
void session_uninit () ;

SSH_SESSION *session_get (char *key) ;
gboolean session_spawn (SSH_SESSION *session, char *hostname, char *username, int rows, int cols) ;
void session_return (SSH_SESSION *the_session) ;
void session_remove (SSH_SESSION *the_session) ;

G_END_DECLS

#endif /* ndef _SESSION_H_ */
