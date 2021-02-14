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

#ifndef _SCREEN_BUFFER_H_
#define _SCREEN_BUFFER_H_

#include <glib.h>
#include "exp_array.h"

G_BEGIN_DECLS

typedef struct _SCREEN_BUFFER SCREEN_BUFFER ;

#define MIN_ROWS  10
#define MAX_ROWS 150

#define MIN_COLS  40
#define MAX_COLS 160

SCREEN_BUFFER *screen_buffer_new (int rows, int cols) ;

void screen_buffer_interpret (SCREEN_BUFFER *screen_buffer, EXP_ARRAY *bucket) ;

gboolean screen_buffer_write_to_socket (SCREEN_BUFFER *screen_buffer, int the_socket) ;

SCREEN_BUFFER *screen_buffer_free (SCREEN_BUFFER *screen_buffer) ;

G_END_DECLS

#endif /* def _SCREEN_BUFFER_H_ */
