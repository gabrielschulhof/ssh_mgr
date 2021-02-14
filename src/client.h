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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <glib.h>

G_BEGIN_DECLS

gpointer client_thread (gpointer data) ;

G_END_DECLS

#endif /* ndef _CLIENT_H_ */
