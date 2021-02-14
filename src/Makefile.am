# This file is part of ssh_mgr2
# Copyright (C) 2006 Gabriel Schulhof <nix@go-nix.ca>
# Package ssh_mgr2 released under GPL (http://www.gnu.org/licenses/gpl.txt):
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along
#  with this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  

## Process this file with automake to produce Makefile.in

# AUTOMAKE_OPTIONS = subdir-objects

# lib_LIBRARIES = libqcadesigner.a
bin_PROGRAMS = ssh_mgr

#################### ssh_mgr ####################
ssh_mgr_DEFINES = \
		$(NULL)

# This variable is only used in Windoze
ssh_mgr_LIBS =	\
		-lglib-2.0    \
		-lgthread-2.0 \
		$(NULL)

ssh_mgr_CFLAGS = \
		$(ssh_mgr_DEFINES)                                         \
		-DPACKAGE_DATA_DIR=\""$(datadir)"\"                        \
		-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
		@GLIB_CFLAGS@                                              \
		@GTHREAD_CFLAGS@                                           \
		$(NULL)

ssh_mgr_SOURCES = \
		exp_array.c             exp_array.h             \
		session.c               session.h               \
		screen_buffer.c         screen_buffer.h         \
		client.c                client.h                \
		screen_buffer_private.c screen_buffer_private.h \
		io.c                    io.h                    \
		main.c                                          \
		$(NULL)

ssh_mgr_LDADD = \
	-lm            \
	-lutil         \
	@GLIB_LIBS@    \
	@GTHREAD_LIBS@ \
	$(NULL)
