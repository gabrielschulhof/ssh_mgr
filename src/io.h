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

#ifndef _IO_H_
#define _IO_H_

#include <glib.h>
#include "exp_array.h"

G_BEGIN_DECLS

// Amount of time (in s) to spend grabbing keystrokes from the socket
#define GRAB_KEYSTROKE_TIME 0.1
#define GRAB_KEYSTROKE_INIT_TIME 1.0

typedef struct _IO_WRAPPER IO_WRAPPER ;

IO_WRAPPER *io_wrap_fd (int fd) ;

char *io_wrapper_read_tag (IO_WRAPPER *io_wrapper, char *tag) ;

EXP_ARRAY *io_wrapper_read_keystrokes (IO_WRAPPER *io_wrapper) ;

// This function always returns NULL
IO_WRAPPER *io_wrapper_close_fd (IO_WRAPPER *io_wrapper) ;

G_END_DECLS

#endif /* ndef _IO_H_ */
