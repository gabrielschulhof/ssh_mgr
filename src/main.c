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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <glib.h>
#include "session.h"
#include "client.h"

#define LISTENING_SOCKET "/tmp/ssh_mgr"

typedef struct
  {
  gboolean show_version ;
  gboolean be_daemon ;
  } CMDLINE_OPTIONS ;

void DoSigTerm (int) ;

static CMDLINE_OPTIONS cmdline =
  {
  .show_version = FALSE,
  .be_daemon    = FALSE
  } ;

static GOptionEntry cmdline_options[] =
  {
  {"version", 'v', 0, G_OPTION_ARG_NONE, &(cmdline.show_version), "Display version number.", NULL},
  {"daemon",  'd', 0, G_OPTION_ARG_NONE, &(cmdline.be_daemon),    "Run in the background.",  NULL},
  {NULL}
  } ;

int main (int argc, char **argv)
  {
  GError *error = NULL ;
  int listening_socket = -1, client_socket = -1 ;
  unsigned int size_of_sockaddr = 0 ;
  struct sockaddr sun ;
  GOptionContext *cmdline_option_parser = NULL ;

  if (NULL != (cmdline_option_parser = g_option_context_new ("- SSH session and TTY manager")))
    {
    g_option_context_add_main_entries (cmdline_option_parser, cmdline_options, ""/*GETTEXT_PACKAGE*/) ;
    g_option_context_parse (cmdline_option_parser, &argc, &argv, NULL) ;
    g_option_context_free (cmdline_option_parser) ;
    }

  if (cmdline.show_version)
    {
    g_print (PACKAGE " " VERSION ", Copyright (C) 2006 Gabriel Schulhof <nix@go-nix.ca>\n"
             "Distributed under GPL (http://www.gnu.org/licenses/gpl.txt)\n") ;
    return 0 ;
    }

#ifndef G_THREADS_ENABLED
  g_error ("gthread is not threadsafe!\n") ;
  exit (1) ;
#endif
  g_thread_init (NULL) ;

if (cmdline.be_daemon)
  {
  /* detach from tty */
  if (fork () > 0) exit (0) ;
  ioctl (0, TIOCNOTTY) ;
  setsid () ;
  }

  signal (SIGTERM, DoSigTerm) ;
  signal (SIGINT, DoSigTerm) ;
  signal (SIGPIPE, SIG_IGN) ;

  if (-1 == (listening_socket = socket (AF_UNIX, SOCK_STREAM, 0)))
    {
    g_error ("socket() failed.\n") ;
    exit (1) ;
    }

  sun.sa_family = AF_UNIX ;
  g_snprintf (sun.sa_data, 13, "%s", LISTENING_SOCKET)  ;

  if (bind (listening_socket, &sun, sizeof (struct sockaddr)))
    {
    g_error ("bind() failed.\n") ;
    exit (1) ;
    }

  if (listen (listening_socket, 10))
    {
    g_error ("listen() failed.\n") ;
    exit (1) ;
    }
  if (chmod (sun.sa_data, 
    S_ISVTX | S_IRUSR | S_IWUSR | S_IXUSR | 
              S_IRGRP | S_IWGRP | S_IXGRP | 
              S_IROTH | S_IWOTH | S_IXOTH))
  g_error ("chmod() failed.\n") ;

  session_init () ;

  while (TRUE)
    if (-1 != (client_socket = accept (listening_socket, NULL, &size_of_sockaddr)))
      {
      g_thread_create (client_thread, GINT_TO_POINTER (client_socket), FALSE, &error) ;
      if (NULL != error)
        g_error ("g_thread_create failed: %s\n", error->message) ;
      }

  return 0 ;
  }

void DoSigTerm (int iSig)
    {
    session_uninit () ;
    fprintf (stderr, "Removing /tmp/ssh_mgr...\n") ;
    unlink ("/tmp/ssh_mgr") ;
    exit (0) ;
    }
