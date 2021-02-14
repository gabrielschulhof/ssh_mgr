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
#include <string.h>
#include <glib.h>
//#include <libxml/tree.h>
#include "screen_buffer.h"
#include "exp_array.h"
#include "screen_buffer_private.h"

#define D_SB(s)
#define D_SB_READ_CHAR(s)
#define D_SB_UTF8(s)

// Initial cell properties
static CELL default_cell = 
  {
  .c                   = ' '        ,
  .fg_clr              = CLR_LTGRAY ,
  .bg_clr              = CLR_BLACK  ,
  .special             = SPSET_NONE ,
  .line_drawing_status = LINES_NONE
  } ;

typedef struct
  {
  Special old_special ;
  int old_fg_clr ;
  } TAG_STACK ;

// Index into the cell grid based on the screen buffer's dimensions
#define CURRENT_POSITION(screen_buffer) ((((screen_buffer)->current_screen)->current_row) * ((screen_buffer)->cols) + (((screen_buffer)->current_screen)->current_col))
// The cell's fore- and background colours depend on whether reverse video is on
#define CELL_FG_CLR(cell) ((((cell).special & SPSET_INVS) ? ((cell).bg_clr) : ((cell).fg_clr)))
#define CELL_BG_CLR(cell) \
  ((0x2592 == (cell).c || \
    0x25ae == (cell).c) ? cell_bg_clr ((&(cell)))  : \
   ((cell).special & SPSET_INVS) ? ((cell).fg_clr) : \
   ((cell).bg_clr))

static SCREEN *screen_new (int rows, int cols) ;
static gboolean screen_write_to_socket (SCREEN *screen, int rows, int cols, int the_socket) ;
static SCREEN *screen_free (SCREEN *screen) ;
static void screen_increment_current_row (SCREEN *screen, int rows, int cols, int how_many) ;
static gboolean create_cell (int the_socket, TAG_STACK *stack, CELL *the_grid, int row_idx, int *col_idx, int cols, int current_row, int current_col, gboolean cursor_visible) ;
static gunichar read_char_from_bucket (EXP_ARRAY *bucket, int *p_idx) ;
static int cell_bg_clr (CELL *cell) ;

SCREEN_BUFFER *screen_buffer_new (int rows, int cols)
  {
  SCREEN_BUFFER *screen_buffer = NULL ;

  if (rows < MIN_ROWS || rows > MAX_ROWS || cols < MIN_COLS || cols > MAX_COLS) return NULL ;
  if (NULL == (screen_buffer = g_malloc0 (sizeof (SCREEN_BUFFER)))) return NULL ;

  screen_buffer->rows = rows ;
  screen_buffer->cols = cols ;
  if (NULL == (screen_buffer->bucket = exp_array_new (sizeof (char), 1)))
    {
    g_free (screen_buffer) ;
    return NULL ;
    }

  if (NULL == (screen_buffer->prm_screen = screen_new (rows, cols)))
    {
    exp_array_free (screen_buffer->bucket) ;
    g_free (screen_buffer) ;
    return NULL ;
    }

  if (NULL == (screen_buffer->alt_screen = screen_new (rows, cols)))
    {
    screen_free (screen_buffer->prm_screen) ;
    exp_array_free (screen_buffer->bucket) ;
    g_free (screen_buffer) ;
    return NULL ;
    }

  screen_buffer->current_screen = screen_buffer->prm_screen ;

  return screen_buffer ;
  }

// This function always returns NULL
SCREEN_BUFFER *screen_buffer_free (SCREEN_BUFFER *screen_buffer)
  {
  if (NULL == screen_buffer) return NULL ;

  exp_array_free (screen_buffer->bucket) ;
  g_free (screen_buffer->prm_screen) ;
  g_free (screen_buffer->alt_screen) ;
  g_free (screen_buffer) ;

  return NULL ;
  }

void screen_buffer_interpret (SCREEN_BUFFER *screen_buffer, EXP_ARRAY *bucket)
  {
  gunichar current_char = 0 ;
  SCREEN *screen = NULL ;
  int Nix ;
  EscapeSequenceResult esc_result = ESC_SUCCESS ;

  if (NULL == screen_buffer || NULL == bucket) return ;

  if (0 == bucket->icUsed) return ;

  // Copy the stuff from the incoming bucket into the bucket associated with the screen_buffer
  exp_array_1d_insert_vals (screen_buffer->bucket, &exp_array_index_1d (bucket, char, 0), bucket->icUsed, -1) ;

  // Interpret as much of the bucket as possible
  for (Nix = 0 ; Nix < screen_buffer->bucket->icUsed ; Nix++)
    {
    while (033 == exp_array_index_1d (screen_buffer->bucket, char, Nix))
      if (ESC_SUCCESS != (esc_result = interpret_escape (screen_buffer, &Nix, &default_cell)))
        break ;

    if (Nix >= screen_buffer->bucket->icUsed || ESC_INCOMPLETE == esc_result) break ;

    screen = screen_buffer->current_screen ;

    if (-2 == (current_char = read_char_from_bucket (screen_buffer->bucket, &Nix)))
      break ;

    //Backspace
    if (8 == current_char)
      {
      if (screen->current_col > 0) (screen->current_col)-- ;
      }
    else
    if ('\n' == current_char)
      screen_increment_current_row (screen, screen_buffer->rows, screen_buffer->cols, 1) ;
    else
    if ('\r' == current_char)
      screen->current_col = 0 ;
    else
    if ('\t' == current_char)
      {
      screen->current_col += (4 - (screen->current_col % 4)) ;
      if (screen->current_col >= screen_buffer->cols)
        screen->current_col = screen_buffer->cols - 1 ;
      }
    else if (14 == current_char)
      screen->current_cell.line_drawing_status = screen->can_line_draw_p ? LINES_ON  : LINES_NONE ;
    else if (15 == current_char)
      screen->current_cell.line_drawing_status = screen->can_line_draw_p ? LINES_OFF : LINES_NONE ;

    // Default character handling behaviour
    else
      {
      // Automatically wrap at the end of the line
      if (screen->current_col == screen_buffer->cols)
        {
        screen->current_col = 0 ;
        screen_increment_current_row (screen, screen_buffer->rows, screen_buffer->cols, 1) ;
        }
      // Ignore bells and '\0' characters
      if (!(7 == current_char || 0 == current_char))
        {
        (screen->the_grid)[CURRENT_POSITION (screen_buffer)]   = screen->current_cell ;
        (screen->the_grid)[CURRENT_POSITION (screen_buffer)].c = current_char ;
        D_SB ({
          if ('q' == (screen->the_grid)[CURRENT_POSITION (screen_buffer)].c)
            g_print ("screen_buffer_interpret: Character '%c' at (r,c) = (%d,%d) will be drawn %s\n",
              (screen->the_grid)[CURRENT_POSITION (screen_buffer)].c, screen->current_row, screen->current_col,
                LINES_ON  == (screen->the_grid)[CURRENT_POSITION (screen_buffer)].line_drawing_status ? "LINES_ON" : 
                LINES_OFF == (screen->the_grid)[CURRENT_POSITION (screen_buffer)].line_drawing_status ? "LINES_OFF" : "LINES_NONE") ;
        })
        
        (screen->current_col)++ ;
        }
      }
    }

  // At this point, Nix is set to the index of the first uninterpreted byte
  if (Nix > screen_buffer->bucket->icUsed)
    Nix = screen_buffer->bucket->icUsed ;
  exp_array_remove_vals (screen_buffer->bucket, 1, 0, Nix) ;
  }

gboolean screen_buffer_write_to_socket (SCREEN_BUFFER *screen_buffer, int the_socket)
  {return screen_write_to_socket (screen_buffer->current_screen, screen_buffer->rows, screen_buffer->cols, the_socket) ;}

static SCREEN *screen_new (int rows, int cols)
  {
  int Nix, Nix1 ;
  SCREEN *screen = g_malloc0 (sizeof (SCREEN)) ;

  if (NULL == screen) return NULL ;

  if (NULL == (screen->the_grid = g_malloc0 (sizeof (CELL) * rows * cols)))
    {
    g_free (screen) ;
    return NULL ;
    }

  screen->current_row       =
  screen->current_col       =
  screen->saved_row         =
  screen->saved_col         = 0 ;
  screen->current_cell      =
  screen->saved_cell        = default_cell ;
  screen->scroll_region_beg = 0 ;
  screen->scroll_region_end = rows - 1 ;
  screen->title_bar_string  = NULL ;
  screen->show_cursor_p     = TRUE ;
  screen->can_line_draw_p   = FALSE ; 

  for (Nix = 0 ; Nix < rows ; Nix++)
    for (Nix1 = 0 ; Nix1 < cols ; Nix1++)
      screen->the_grid[Nix * cols + Nix1] = default_cell ;

  return screen ;
  }

// This function always returns NULL
static SCREEN *screen_free (SCREEN *screen)
  {
  if (NULL == screen) return NULL ;
  g_free (screen->the_grid) ;
  if (NULL != screen->title_bar_string)
    g_free (screen->title_bar_string) ;
  g_free (screen) ;

  return NULL ;
  }

static void screen_increment_current_row (SCREEN *screen, int rows, int cols, int how_many)
  {
  int start_row = 0 ;
  int rows_below_terminal = 0 ;
  int Nix, Nix1 ;

  if (screen->current_row > screen->scroll_region_end)
    {
    screen->current_row = 
      (screen->current_row + how_many >= rows)
        ? rows - 1 : screen->current_row + how_many ;
    return ;
    }

  if (screen->current_row + how_many <= screen->scroll_region_end)
    {
    screen->current_row += how_many ;
    return ;
    }

  if ((rows_below_terminal = screen->current_row + how_many - screen->scroll_region_end) <
      (screen->scroll_region_end - screen->scroll_region_beg + 1))
    {
    memmove (
      &((screen->the_grid)[screen->scroll_region_beg * cols]), // destination
      &((screen->the_grid)[(screen->scroll_region_beg + rows_below_terminal) * cols]), // source
      (screen->scroll_region_end - (rows_below_terminal + screen->scroll_region_beg) + 1) * cols * sizeof (CELL) // quantity
      ) ;
    start_row = screen->scroll_region_end - rows_below_terminal + 1 ;
    }
  else
    start_row = 0 ;

  // Blank out newly "exposed" rows
  for (Nix = start_row ; Nix <= screen->scroll_region_end ; Nix++)
    for (Nix1 = 0 ; Nix1 < cols ; Nix1++)
      screen->the_grid[Nix * cols + Nix1] = default_cell ;

  screen->current_row = screen->scroll_region_end ;
  }

// Write a screen to a socket - basically, in HTML format
// express regions that share a common background as a single cell
static gboolean screen_write_to_socket (SCREEN *screen, int rows, int cols, int the_socket)
  {
  int Nix, Nix1, str_len = -1 ;
  TAG_STACK stack = {SPSET_NONE, -1} ;
  char buffer[32] = "" ;

  D_SB (g_print ("screen_write_to_socket: Entering\n") ;)

  str_len = g_snprintf (buffer, 31, "{%d:-1:", cols) ;
  if (write (the_socket, buffer, str_len) < 0) return FALSE ;
  if (NULL != screen->title_bar_string)
    if (write (the_socket, screen->title_bar_string, strlen (screen->title_bar_string)) < 0) return FALSE ;
  if (write (the_socket, "}\n", 2) < 0) return FALSE ;

  for (Nix = 0 ; Nix < rows ; Nix++)
    {
    D_SB (g_print ("screen_write_to_socket: row %d\n", Nix) ;)

    for (Nix1 = 0 ; Nix1 < cols ; Nix1++)
      if (!create_cell (the_socket, &stack, screen->the_grid, Nix, &Nix1, cols, screen->current_row, screen->current_col, screen->show_cursor_p))
        return FALSE ;

    if (write (the_socket, "\n", 1) < 0) return FALSE ;
    }

  D_SB (g_print ("screen_write_to_socket: Exiting\n") ;)
  return TRUE ;
  }

static gboolean set_specials (int the_socket, TAG_STACK *stack, CELL *the_cell)
  {
  int cell_fg_clr = -1 ;
  Special cell_special = SPSET_NONE ;
  int str_len = 0 ;
  char buffer[33] = "" ;
  Special ar_sp[4] = {SPSET_NONE, SPSET_USCR, SPSET_BLNK, SPSET_BOLD} ;
//  char *ar_sp_str[4] = {NULL, "u", "blink", "b"} ;
  char *ar_sp_str[4] = {NULL, "u", "l", "b"} ;
  int Nix ;

  if (NULL != the_cell)
    {
    cell_fg_clr = CELL_FG_CLR (*the_cell) ;
    if ((the_cell->special & SPSET_BOLD) && cell_fg_clr < 8)
      cell_fg_clr += 8 ;
    cell_special = the_cell->special ;
    }

  for (Nix = 3 ; Nix > -1 ; Nix--)
    if (Nix > 0)
      {
      if ((stack->old_special & ar_sp[Nix]) && (!(cell_special & ar_sp[Nix])))
        {
        str_len = g_snprintf (buffer, 32, "</%s>", ar_sp_str[Nix]) ;
        D_SB (g_print ("set_specials: Writing \"%s\"(%d bytes) to socket\n", buffer, str_len) ;)
        if (write (the_socket, buffer, str_len) < 0) return FALSE ;
        }
      }
    else
    if (stack->old_fg_clr != cell_fg_clr)
      {
      if (write (the_socket, "</c>", 4) < 0) return FALSE ;
      D_SB (g_print ("set_specials: Writing \"%s\"(%d bytes) to socket\n", "</c>", 7) ;)
      }

  if (NULL != the_cell)
    for (Nix = 0 ; Nix < 4 ; Nix++)
      if (Nix > 0)
        {
        if ((!(stack->old_special & ar_sp[Nix])) && (the_cell->special & ar_sp[Nix]))
          {
          str_len = g_snprintf (buffer, 32, "<%s>", ar_sp_str[Nix]) ;
          D_SB (g_print ("set_specials: Writing \"%s\"(%d bytes) to socket\n", buffer, str_len) ;)
          if (write (the_socket, buffer, strlen (buffer)) < 0) return FALSE ;
          }
        }
      else
      if (stack->old_fg_clr != cell_fg_clr)
        {
        str_len = g_snprintf (buffer, 32, "<c%d>", cell_fg_clr) ;
        D_SB (g_print ("set_specials: Writing \"%s\"(%d bytes) to socket\n", buffer, str_len) ;)
        if (write (the_socket, buffer, str_len) < 0) return FALSE ;
        }

  stack->old_fg_clr = cell_fg_clr ;
  stack->old_special = cell_special ;

  return TRUE ;
  }

static gboolean create_cell (int the_socket, TAG_STACK *stack, CELL *the_grid, int row_idx, int *col_idx, int cols, int current_row, int current_col, gboolean show_cursor)
  {
  gboolean cursor_row = (row_idx == current_row) ;
  int idx_end, row_offset = row_idx * cols, idx ;
  int idx_space_check, idx_space ;
  CELL *the_cell = &the_grid[row_offset + (*col_idx)] ;
  char buffer[94] = "" ;
  int col_span = -1 ;
  int bg_clr = (cursor_row && (*col_idx) == current_col && show_cursor) ? 16 : CELL_BG_CLR (*the_cell) ;
  int fg_clr = CELL_FG_CLR (*the_cell) ;
  int str_len = 0 ;
  int new_bg_clr = -1 ;
  gunichar c ;

  if ((the_cell->special & SPSET_BOLD) && fg_clr < 8)
    fg_clr += 8 ;

  for (idx_end = (*col_idx) + 1 ; idx_end < cols ; idx_end++)
    {
    new_bg_clr = (cursor_row && idx_end == current_col && show_cursor) ? 16 : CELL_BG_CLR (the_grid[row_offset + idx_end]) ;
    if (new_bg_clr != bg_clr)
      break ;
    }

  D_SB (g_print("create_cell: current vs. beg_idx vs. end_idx: (%d,%d) vs. (%d,%d) vs. (%d,%d)\n", current_row, current_col, row_idx, (*col_idx), row_idx, idx_end) ;)

  col_span = idx_end - (*col_idx) ;
//  if (col_span > 1)
//    str_len = g_snprintf (buffer, 93, "<td align=\"left\" bgcolor=\"%s\" colspan=\"%d\"><tt><font color=\"%s\">\n", clrs[bg_clr], col_span, clrs[fg_clr]) ;
//  else
//    str_len = g_snprintf (buffer, 93, "<td align=\"left\" bgcolor=\"%s\"><tt><font color=\"%s\">\n", clrs[bg_clr], clrs[fg_clr]) ;

  str_len = g_snprintf (buffer, 93, "{%d:%d:<c%d>", col_span, bg_clr, fg_clr) ;
  D_SB (g_print ("create_cell: Writing \"%s\"(%d bytes) to socket\n", buffer, str_len) ;)
  if (write (the_socket, buffer, str_len) < 0) return FALSE ;

  stack->old_fg_clr = fg_clr ;

  idx_end-- ;

  for (idx = (*col_idx) ; idx <= idx_end ; idx++)
    {
    the_cell = &the_grid[row_offset + idx] ;

    if (!set_specials (the_socket, stack, the_cell)) return FALSE ;

    D_SB ({
      if (LINES_ON == the_cell->line_drawing_status)
        g_print ("create_cell: Writing line drawing cell '%c'\n", the_cell->c) ;
    })

    g_snprintf (buffer, 93, "%02x", c = DO_CHAR (the_cell->c, the_cell->line_drawing_status)) ;

    D_SB_UTF8({
      if (the_cell->c > 0xFF)
        g_print ("UTF8-char: 0x%x, line drawing: %s, resulting char: %c\n", the_cell->c,
          LINES_NONE == the_cell->line_drawing_status ? "LINES_NONE" :
          LINES_OFF  == the_cell->line_drawing_status ? "LINES_OFF"  : "LINES_ON", ((unsigned char)c) > 128 ? '?' : c) ;
    })

    D_SB (g_print ("create_cell: Writing \"%s\"[%c](%d bytes) to socket\n", buffer, c, 2) ;)
//    if (0xb0 == c)
//      {if (write (the_socket, "\n<sup>o</sup>\n", 14)) return FALSE ;}
//    else
    if (write (the_socket, buffer, 2) < 0) return FALSE ;

    if (' ' == c)
      {
      for (idx_space_check = idx + 1 ; idx_space_check <= idx_end ; idx_space_check++)
        if (idx_space_check == current_col && cursor_row)
          break ;
        else
        if (!(the_grid[row_offset + idx].fg_clr              == the_grid[row_offset + idx_space_check].fg_clr  &&
              the_grid[row_offset + idx].bg_clr              == the_grid[row_offset + idx_space_check].bg_clr  &&
              the_grid[row_offset + idx].special             == the_grid[row_offset + idx_space_check].special &&
              the_grid[row_offset + idx].line_drawing_status == the_grid[row_offset + idx_space_check].line_drawing_status))
          break ;
        else
        if (c != DO_CHAR ((the_grid[row_offset + idx_space_check].c), the_grid[row_offset + idx_space_check].line_drawing_status))
          break ;

      // If spaces don't take us to the end of the cell, then we must print them all
      idx_space_check-- ;
      if (idx_space_check < idx_end)
        for (idx_space = idx + 1 ; idx_space <= idx_space_check ; idx_space++)
          if (write (the_socket, "20", 2) < 0) return FALSE ; // 0x20 == space

      idx = idx_space_check ;
      }
    }

  if (!set_specials (the_socket, stack, NULL)) return FALSE ;
  if (write (the_socket, "}", 1) < 0) return FALSE ;

  (*col_idx) = idx_end ;

  return TRUE ;
  }

static gunichar read_char_from_bucket (EXP_ARRAY *bucket, int *p_idx)
  {
  char *str = NULL, *str_new = NULL ;
  gunichar ret ;

  D_SB_READ_CHAR (g_print ("read_char_from_bucket: Entering with (*p_idx) = %d (%c)\n", (*p_idx), 
    exp_array_index_1d (bucket, char, (*p_idx)))) ;

  D_SB_READ_CHAR (g_print ("read_char_from_bucket: bucket->icUsed - (*p_idx) = %d\n", bucket->icUsed - (*p_idx))) ;

  ret = g_utf8_get_char_validated ((str = &exp_array_index_1d (bucket, char, (*p_idx))), bucket->icUsed - (*p_idx)) ;

  D_SB_READ_CHAR (g_print ("read_char_from_bucket: ret = 0x%x\n", ret) ;)

  if (((gunichar)(-1)) == ret)
    ret = (gunichar)(*str) ;
  else
  if (ret >= 0)
    {
    D_SB_READ_CHAR (g_print ("read_char_from_bucket: Looking for str_new between '%c' and '%c'\n", *str, exp_array_index_1d (bucket, char, bucket->icUsed - 1)) ;)
    if (NULL != (str_new = g_utf8_find_next_char (str, &exp_array_index_1d (bucket, char, bucket->icUsed - 1))))
      {
      D_SB_READ_CHAR (g_print ("read_char_from_bucket: *str_new = %c\n", *str_new) ;)
      (*p_idx) += (str_new - str) - 1 ;
      }
    }

	g_print("read_char_from_bucket: 0x%08x\n", ret);

  return ret ;
  }

static int cell_bg_clr (CELL *cell)
  {
  int min_clr = -1, max_clr = -1 ;

  if (cell->fg_clr < cell->bg_clr)
    {
    min_clr = cell->fg_clr ;
    max_clr = cell->bg_clr ;
    }
  else
    {
    min_clr = cell->bg_clr ;
    max_clr = cell->fg_clr ;
    }

  // 136 - (((16 - min_clr) * (17 - min_clr)) >> 1) is the idx at the start of the row[min_clr]
  // max_clr - min_clr is the column of the row[min_clr]
  //+ 17 is the offset for the first bunch of colours (added to 136 to give the inital 153)
  return 153 - (((16 - min_clr) * (17 - min_clr)) >> 1) + max_clr - min_clr ;
  }
