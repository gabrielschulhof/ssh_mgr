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

#ifndef _SCREEN_BUFFER_PRIVATE_H_
#define _SCREEN_BUFFER_PRIVATE_H_

#include <glib.h>
#include "exp_array.h"
#include "screen_buffer.h"

G_BEGIN_DECLS

typedef enum
  {
  CLR_BLACK = 0,
  CLR_RED,
  CLR_GREEN,
  CLR_BROWN,
  CLR_BLUE,
  CLR_PURPLE,
  CLR_CYAN,
  CLR_LTGRAY,
  CLR_DKGRAY,
  CLR_LTRED,
  CLR_LTGREEN,
  CLR_YELLOW,
  CLR_LTBLUE,
  CLR_LTPURPLE,
  CLR_LTCYAN,
  CLR_WHITE,
  CLR_CUSTOM
  } Colour ;

typedef enum
  {
  SPSET_NONE = 0,
  SPSET_USCR = 1 << 0,
  SPSET_BLNK = 1 << 1,
  SPSET_INVS = 1 << 2,
  SPSET_HIDN = 1 << 3,
  SPSET_BOLD = 1 << 4
  } Special ;

typedef enum
  {
  LINES_OFF = 0,
  LINES_ON,
  LINES_NONE
  } LineDrawingStatus ;

typedef enum
  {
  ESC_SUCCESS = 0,
  ESC_FAILURE,
  ESC_INCOMPLETE
  } EscapeSequenceResult ;

typedef struct
  {
  gunichar c ;
  Colour fg_clr ;
  Colour bg_clr ;
  Special special ;
  LineDrawingStatus line_drawing_status ;
  } CELL ;

typedef struct
  {
  CELL *the_grid ;

  int current_row ;
  int current_col ;
  CELL current_cell ;

  int saved_row ;
  int saved_col ;
  CELL saved_cell ;

  int scroll_region_beg ;
  int scroll_region_end ;

  char *title_bar_string ;

  gboolean show_cursor_p ;
  gboolean can_line_draw_p ;
  } SCREEN ;

struct _SCREEN_BUFFER
  {
  int rows ;
  int cols ;
  EXP_ARRAY *bucket ;
  SCREEN *prm_screen ;
  SCREEN *alt_screen ;
  SCREEN *current_screen ;
  } ;

// Character mangling - replace line drawing characters with (poor?) ASCII art equivalents
#define DO_CHAR(chr,lds)                                               \
  ((unsigned char)((0x00da == (chr) || 0x00c3 == (chr) ||              \
                    0x00bf == (chr) || 0x00b4 == (chr) ||              \
                    0x00d9 == (chr) ||                                 \
                    0x00c0 == (chr))  && LINES_NONE != (lds)) ? '+'  : \
                  ((0x00b3 == (chr)   && LINES_NONE != (lds)) ? '|'  : \
                   (0x00c4 == (chr)   && LINES_NONE != (lds)) ? '-'  : \
                   (0x0018 == (chr)   && LINES_NONE != (lds)) ? '^'  : \
                   (0x0019 == (chr)   && LINES_NONE != (lds)) ? 'v'  : \
                     (('l' == (chr) ||    'k' == (chr) ||              \
                       'm' == (chr) ||    'j' == (chr) ||              \
                       'u' == (chr) ||                                 \
                       't' == (chr))  && LINES_ON   == (lds)) ? '+'  : \
                      ('x' == (chr)   && LINES_ON   == (lds)) ? '|'  : \
                      ('q' == (chr)   && LINES_ON   == (lds)) ? '-'  : \
                      ('s' == (chr)   && LINES_ON   == (lds)) ? '_'  : \
/* degree symbol */   ('f' == (chr)   && LINES_ON   == (lds)) ? 0xb0 : \
/* +/- symbol */      ('g' == (chr)   && LINES_ON   == (lds)) ? 0xb1 : \
                  ((                                                   \
									  0x250c == (chr) || 0x2524 == (chr) ||              \
                    0x251c == (chr) || 0x2518 == (chr) ||              \
                    0x2514 == (chr) || 0x253c == (chr) ||              \
                    0x2554 == (chr) || 0x2557 == (chr) ||              \
                    0x255a == (chr) || 0x255d == (chr) ||              \
                    0x255e == (chr) || 0x2562 == (chr) ||              \
                    0x255f == (chr) || 0x252c == (chr) ||              \
                    0x2534 == (chr) || 0x256d == (chr) ||              \
                    0x256e == (chr) || 0x256f == (chr) ||              \
                    0x2570 == (chr) || 0x256f == (chr) ||              \
                    0x2568 == (chr) || 0x2565 == (chr) ||              \
                    0x2569 == (chr) || 0x2567 == (chr) ||              \
                    0x2552 == (chr) || 0x2555 == (chr) ||              \
                    0x255b == (chr) || 0x2558 == (chr) ||              \
                    0x2553 == (chr) || 0x2556 == (chr) ||              \
                    0x255c == (chr) || 0x2559 == (chr) ||              \
                    0x2560 == (chr) || 0x2561 == (chr) ||              \
                    0x2563 == (chr) || 0x2566 == (chr) ||              \
                    0x2564 == (chr) || 0x2564 == (chr) ||              \
                    0x2510 == (chr)                                    \
										)  && LINES_NONE == (lds)) ? '+'  :                \
                  ((0x2502 == (chr) || 0x2503 == (chr) ||              \
									  0x2551 == (chr) || 0x257f == (chr) ||              \
									  0x257d == (chr) ||                                 \
									  0x2510 == (chr))  && LINES_NONE == (lds)) ? '|'  : \
                   (0x2500 == (chr)   && LINES_NONE == (lds)) ? '-'  : \
                   (0x2191 == (chr)   && LINES_NONE == (lds)) ? '^'  : \
                   (0x2193 == (chr)   && LINES_NONE == (lds)) ? 'v'  : \
                   (0x2550 == (chr)   && LINES_NONE == (lds)) ? '='  : \
/* 50% shade box*/ (0x2592 == (chr)   && LINES_NONE == (lds)) ? ' '  : \
/* scrollbar box*/ (0x25ae == (chr)   && LINES_NONE == (lds)) ? '#'  : \
/* backslash */    (0x2572 == (chr)   && LINES_NONE == (lds)) ? 0x5c : \
                   (0x2571 == (chr)   && LINES_NONE == (lds)) ? '/'  : \
                   (0x2573 == (chr)   && LINES_NONE == (lds)) ? 'X'  : \
                   (chr)))

EscapeSequenceResult interpret_escape (SCREEN_BUFFER *screen_buffer, int *pidx, CELL *default_cell) ;

G_END_DECLS

#endif /* _SCREEN_BUFFER_PRIVATE_H_ */
