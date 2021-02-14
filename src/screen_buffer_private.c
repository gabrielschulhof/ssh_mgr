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

#include <string.h>
#include <stdlib.h>
#include "screen_buffer_private.h"

static gboolean find_ending_char (EXP_ARRAY *bucket, int idx, int *p_end_idx) ;
static int read_int (EXP_ARRAY *bucket, int idx, int *p_new_idx, int default_val) ;

#define D_ESC_BUCKET(s)
#define D_ESC_SCREEN(s)
#define D_ESC(s)
#define D_ESC_READ_INT(s)

#define IS_NUMERIC(c) (((c) >= '0') && ((c) <= '9'))

#define SPECIAL_NONE 0
#define SPECIAL_BOLD 1
#define SPECIAL_USCR 4
#define SPECIAL_BLNK 5
#define SPECIAL_INVS 7
#define SPECIAL_HIDN 8

EscapeSequenceResult interpret_escape (SCREEN_BUFFER *screen_buffer, int *pidx, CELL *default_cell)
  {
  EscapeSequenceResult esc_ret = ESC_FAILURE ;
  int number_read = -1 ;
  int beg_char_idx = -1 ;
  int end_char_idx = -1 ;
  SCREEN *screen = NULL ;
  CELL *the_grid = NULL ;

  if (NULL == screen_buffer || NULL == pidx) return ESC_FAILURE ;

  if ((*pidx) >= screen_buffer->bucket->icUsed) return ESC_INCOMPLETE ;

  screen = screen_buffer->current_screen ;
  the_grid = screen->the_grid ;

  D_ESC_SCREEN({
    int idx_scr_row ;
    int idx_scr_col ;
    g_print ("interpret_escape: screen is:\n") ;
    g_print ("++++++++++++++++++++++++++++++++++\n") ;
    for (idx_scr_row = 0 ; idx_scr_row < screen_buffer->rows ; idx_scr_row++)
      {
      for (idx_scr_col = 0 ; idx_scr_col < screen_buffer->cols ; idx_scr_col++)
        g_print ("%c", DO_CHAR (the_grid[idx_scr_row * screen_buffer->cols + idx_scr_col].c, the_grid[idx_scr_row * screen_buffer->cols + idx_scr_col].line_drawing_status)) ;
      g_print ("\n") ;
      }
    g_print ("++++++++++++++++++++++++++++++++++\n") ;
  })

  D_ESC_BUCKET (g_print ("interpret_escape: bucket is:\n") ;)
  D_ESC_BUCKET (fwrite (&exp_array_index_1d (screen_buffer->bucket, char, (*pidx)), (screen_buffer->bucket->icUsed) - (*pidx), 1, stdout) ;)
  D_ESC_BUCKET (g_print ("\n==================================\n") ;)
  if (!find_ending_char (screen_buffer->bucket, beg_char_idx = (*pidx) + 1, &end_char_idx))
    D_ESC ({)
    D_ESC (g_print ("interpret_escape: Failed to find ending char\n") ;)
    return ESC_INCOMPLETE ;
    D_ESC (})
  else
    (*pidx) = end_char_idx + 1 ;

  D_ESC ({
    g_print ("interpret_escape: beg_char_idx = %d[%c], end_char_idx = %d[%c]",
      beg_char_idx, exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx), 
      end_char_idx, exp_array_index_1d (screen_buffer->bucket, char, end_char_idx)) ;
    if ((*pidx) < screen_buffer->bucket->icUsed)
      g_print (", (*pidx) = %d[%c]\n",
        (*pidx),    exp_array_index_1d (screen_buffer->bucket, char, (*pidx))) ;
    else
      g_print (", (*pidx) = <past end>\n") ;
    })

  // At this point, pidx points past the end of this ESC sequence, even though we haven't successfully interpreted it yet

  switch (exp_array_index_1d (screen_buffer->bucket, char, end_char_idx))
    {
    case 007: // xterm title bar
      {
      int idx_title_beg = -1, idx_title_end = -1 ;

      D_ESC (g_print ("interpret_escape: CASE: title: ") ;) ;

      if (beg_char_idx < end_char_idx - 1)
        if ('0' == exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx + 1) ||
            '2' == exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx + 1))
          if ((idx_title_beg = beg_char_idx + 2) < end_char_idx)
            if (';' == exp_array_index_1d (screen_buffer->bucket, char, idx_title_beg))
              if (++idx_title_beg > end_char_idx)
                {
                D_ESC (g_print ("invalid ';' sequence\n") ;)
                esc_ret = ESC_SUCCESS ;
                break ;
                }

      idx_title_end = end_char_idx - 1 ;

      D_ESC ({
        if (idx_title_beg <= end_char_idx && idx_title_end <= end_char_idx)
          g_print ("valid sequence: beg = '%c' end = '%c': ",
            exp_array_index_1d (screen_buffer->bucket, char, idx_title_beg),
            exp_array_index_1d (screen_buffer->bucket, char, idx_title_end)) ;
        else
          g_print ("invalid sequence") ;
      })

      // If the 2 indices are reversed by at most one byte, the sequence is valid, so remove the old title
      if (idx_title_beg <= idx_title_end + 1)
        {
        if (NULL != screen->title_bar_string)
          g_free (screen->title_bar_string) ;
        screen->title_bar_string = NULL ;

        // If there's a new title, set it
        if (idx_title_beg <= idx_title_end)
          screen->title_bar_string = g_strndup (&exp_array_index_1d (screen_buffer->bucket, char, idx_title_beg), idx_title_end - idx_title_beg + 1) ;
        esc_ret = ESC_SUCCESS ;
        }
      D_ESC (g_print ("\n") ;)
      break ;
      }

    case '=': // numlock message  ... ignored
    case '>': // numlock message  ... ignored
    case 'g': // [0g == clear current tab col, [3g == clear all tabs
      esc_ret = ESC_SUCCESS ;
      break ;

    case '@': // insert characters
      {
      int Nix, row_offset = screen->current_row * screen_buffer->cols ;

      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      number_read = MIN (number_read, screen_buffer->cols - screen->current_col) ;

      if (screen_buffer->cols - screen->current_col - number_read > 0)
        memmove (
          &the_grid[row_offset + screen->current_col + number_read],
          &the_grid[row_offset + screen->current_col],
          (screen_buffer->cols - screen->current_col - number_read) * sizeof (CELL)) ;

      for (Nix = screen->current_col ; Nix < screen->current_col + number_read ; Nix++)
        the_grid[row_offset + Nix] = screen->current_cell ;

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case '(':
    case ')':
      D_ESC (g_print ("interpret_escape: CASE: can_line_draw_p: ") ;)
      if (end_char_idx + 2 <= screen_buffer->bucket->icUsed)
        {
        if ('0' == exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx))
          {
          D_ESC (g_print ("switching from %s to %s\n", screen->can_line_draw_p ? "TRUE" : "FALSE", "TRUE") ;)
          screen->can_line_draw_p = TRUE ;
          if ('(' == exp_array_index_1d (screen_buffer->bucket, char, end_char_idx))
            screen->current_cell.line_drawing_status = LINES_ON ;
          }
        else
        if ('A' == exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx) ||
            'B' == exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx))
          {
          D_ESC (g_print ("switching from %s to %s\n", screen->can_line_draw_p ? "TRUE" : "FALSE", "FALSE") ;)
          screen->can_line_draw_p = FALSE ;
          screen->current_cell.line_drawing_status = LINES_NONE ;
          }

        esc_ret = ESC_SUCCESS ;
        }
      break ;

    case '+':
    case '*':
      if (end_char_idx + 2 <= screen_buffer->bucket->icUsed)
        {
        (*pidx)++ ; // Why must we point /two/ past the end of the ESC ?
        esc_ret = ESC_SUCCESS ;
        }
      break ;

    case '7': // Save cursor
      screen->saved_row  = screen->current_row ;
      screen->saved_col  = screen->current_col ;
      screen->saved_cell = screen->current_cell ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case '8': // Restore cursor
      screen->current_row  = screen->saved_row ;
      screen->current_col  = screen->saved_col ;
      screen->current_cell = screen->saved_cell ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'A': // Move n rows up
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      screen->current_row -= number_read ;
      if (screen->current_row < 0)
        screen->current_row = 0 ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'B': // Move n rows down
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      screen->current_row += number_read ;
      if (screen->current_row >= screen_buffer->rows)
        screen->current_row = screen_buffer->rows - 1 ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'C': // Advance n columns
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      screen->current_col += number_read ;
      if (screen->current_col >= screen_buffer->cols)
        screen->current_col = screen_buffer->cols - 1 ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'D': // Retreat n columns
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      screen->current_col -= number_read ;
      if (screen->current_col < 0)
        screen->current_col = 0 ;
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'P':
      {
      int Nix, row_offset = -1 ;

      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      // figure out how many cols we can delete
      number_read = MIN (number_read, screen_buffer->cols - screen->current_col) ;
      row_offset  = screen->current_row * screen_buffer->cols ;
      if ((screen_buffer->cols - (screen->current_col + number_read)) > 0)
        memmove (&the_grid[row_offset + screen->current_col],
                 &the_grid[row_offset + screen->current_col + number_read],
                 (screen_buffer->cols - (screen->current_col + number_read)) * sizeof (CELL)) ;
      for (Nix = MAX (screen->current_col, screen_buffer->cols - number_read) ; Nix < screen_buffer->cols ; Nix++)
        the_grid[row_offset + Nix] = screen->current_cell ;
      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'G': // Go to column n
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, screen->current_col + 1) - 1 ;
      D_ESC (g_print ("interpret_escape: CASE: Go to column: %d -> ", screen->current_col) ;)
      screen->current_col = CLAMP (number_read, 0, screen_buffer->cols - 1) ;
      D_ESC (g_print ("%d\n", screen->current_col) ;)
      esc_ret = ESC_SUCCESS ;
      break ;

    case 'f':
    case 'H': // Absolute (r,c) reference
      {
      int next_idx = beg_char_idx + 1 ;

      D_ESC (g_print ("interpret_escape: CASE: absolute (r,c) reference: Moving from (%d,%d) ", screen->current_row, screen->current_col) ;)

      if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, next_idx)))
        {
        number_read = read_int (screen_buffer->bucket, next_idx, &next_idx, 1) - 1 ;
        screen->current_row = CLAMP (number_read, 0, screen_buffer->rows - 1) ;
        }
      else // No coords specified, so go home
        screen->current_row = 
        screen->current_col = 0 ;

      if (next_idx + 1 < end_char_idx) next_idx++ ;

      D_ESC (g_print ("reading second coord starting at '%c' ... ", exp_array_index_1d (screen_buffer->bucket, char, next_idx)) ;)
      number_read = read_int (screen_buffer->bucket, next_idx, &next_idx, screen->current_col + 1) - 1 ;
      screen->current_col = CLAMP (number_read, 0, screen_buffer->cols - 1) ;

      D_ESC (g_print ("to (%d,%d)\n", screen->current_row, screen->current_col) ;)

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'J': // \033[2J == clear screen, but the '2' seems optional
      {
      int Nix, Nix1, row_offset = -1 ;

      D_ESC (g_print ("interpret_escape: CASE: clear screen\n") ;)

      for (Nix = 0 ; Nix < screen_buffer->rows ; Nix++)
        {
        row_offset = Nix * screen_buffer->cols ;
        for (Nix1 = 0 ; Nix1 < screen_buffer->cols ; Nix1++)
          the_grid[row_offset + Nix1] = screen->current_cell ;
        }
      screen->current_row = 
      screen->current_col = 0 ;
      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'K': // Erase parts of the current line
      {
      int erase_beg = -1, erase_end = -1, Nix, row_offset = -1 ;

      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 0) ;
      D_ESC (g_print ("interpret_escape: CASE: erase parts of the current line: ") ;)

      if (0 == number_read)
        {
        erase_beg = screen->current_col ;
        erase_end = screen_buffer->cols ;
        }
      else
      if (1 == number_read)
        {
        erase_beg = 0 ;
        erase_end = screen->current_col + 1 ;
        }
      else
      if (2 == number_read)
        {
        erase_beg = 0 ;
        erase_end = screen_buffer->cols ;
        }
      else
        {
        D_ESC(g_print ("ignoring\n") ;)
        esc_ret = ESC_SUCCESS ; // Ignore this ESC if there's an invalid value
        break ;
        }

      D_ESC (g_print ("erasing idx range (%d->%d)\n", erase_beg, erase_end) ;)

      row_offset = screen->current_row * screen_buffer->cols ;
      for (Nix = erase_beg ; Nix < erase_end ; Nix++)
        the_grid[row_offset + Nix] = screen->current_cell ;

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'L': // Insert n lines
      {
      int end_row_idx = -1, Nix, Nix1, row_offset = -1 ;

      // Does nothing if the cursor isn't inside the scrolling region
      if (screen->current_row >= screen->scroll_region_beg && screen->current_row <= screen->scroll_region_end)
        {
        number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;

        if (screen->current_row + number_read > screen->scroll_region_end)
          end_row_idx = screen->scroll_region_end ;
        else
          {
          memmove (
            &(the_grid[(screen->current_row + number_read) * screen_buffer->cols]),
            &(the_grid[screen->current_row * screen_buffer->cols]),
            (screen->scroll_region_end - (screen->current_row + number_read) + 1) * screen_buffer->cols * sizeof (CELL)
            ) ;
          end_row_idx = screen->current_row + number_read - 1 ;
          }

        for (Nix = screen->current_row ; Nix <= end_row_idx ; Nix++)
          {
          row_offset = Nix * screen_buffer->cols ;
          for (Nix1 = 0 ; Nix1 < screen_buffer->cols ; Nix1++)
            the_grid[row_offset + Nix1] = screen->current_cell ;
          }
        }

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'M':
      {
      int row_offset = -1 ;

      // Move up one line through the scrolling region and, if necessary, scroll down to stay within it
      if (exp_array_index_1d (screen_buffer->bucket, char, beg_char_idx) == 
          exp_array_index_1d (screen_buffer->bucket, char, end_char_idx))
        {
        D_ESC (g_print ("interpret_escape: Moving up one line and, if at the start of the scroll buffer, pushing scroll buffer down\n") ;)
        D_ESC (g_print ("    scroll buffer is (%d<-->%d), current_row = %d\n", screen->scroll_region_beg, screen->scroll_region_end, screen->current_row) ;)

        // The following may seem weird, but think of the case where screen->scroll_region_beg == screen->scroll_region_end
        if (screen->current_row == screen->scroll_region_end || screen->current_row != screen->scroll_region_beg)
          {
          if (screen->current_row > 0)
            (screen->current_row)-- ;
          }
        else // screen->current_row != screen->scroll_region_end && screen->current_row == screen->scroll_region_beg
          {
          int Nix ;

          // We know that screen->scroll_region_beg is not the last row and we have a proper scroll_region
          memmove (&the_grid[(screen->current_row + 1) * screen_buffer->cols],
                   &the_grid[screen->current_row * screen_buffer->cols],
                   (screen->scroll_region_end - screen->scroll_region_beg) * screen_buffer->cols * sizeof (CELL)) ;

          row_offset = screen->current_row * screen_buffer->cols ;
          for (Nix = 0 ; Nix < screen_buffer->cols ; Nix++)
            the_grid[row_offset + Nix] = screen->current_cell ;
          }
        }
      // Delete n lines and scroll the rest of them up
      else
      if (screen->current_row >= screen->scroll_region_beg && screen->current_row <= screen->scroll_region_end)
        {
        int Nix, Nix1 ;
        number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
        number_read = MIN (number_read, screen->scroll_region_end - screen->current_row + 1) ;

        D_ESC (g_print ("interpret_escape: Deleting %d lines and scrolling up: scroll region is (%d<-->%d)\n", number_read, screen->scroll_region_beg, screen->scroll_region_end) ;)

        if (screen->current_row < screen->scroll_region_end)
          memmove (
            &(the_grid[screen->current_row * screen_buffer->cols]),
            &(the_grid[(screen->current_row + number_read) * screen_buffer->cols]),
            (screen->scroll_region_end - screen->scroll_region_beg - number_read + 1) * screen_buffer->cols * sizeof (CELL)
            ) ;

        for (Nix = screen->scroll_region_end - number_read + 1 ; Nix <= screen->scroll_region_end ; Nix++)
          {
          row_offset = Nix * screen_buffer->cols ;
          for (Nix1 = 0 ; Nix1 < screen_buffer->cols ; Nix1++)
            the_grid[row_offset + Nix1] = screen->current_cell ;
          }
        }

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'S': // scroll the scrolling region up n lines
      {
      int Nix, Nix1, row_offset = -1 ;
      int real_scroll_region_beg = screen->scroll_region_beg, 
          real_scroll_region_end = screen->scroll_region_end, number_of_scroll_rows = 0 ;

      if (screen->scroll_region_beg == screen->scroll_region_end)
        {
        real_scroll_region_beg = 0 ;
        real_scroll_region_end = screen_buffer->rows - 1 ;
        }

      number_of_scroll_rows = real_scroll_region_end - real_scroll_region_beg + 1 ;
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      number_read = MIN (number_read, number_of_scroll_rows) ;

      D_ESC (g_print ("interpret_escape: CASE: Scroll buffer (%d<-->%d) =%d lines forward %d lines\n", real_scroll_region_beg, real_scroll_region_end, number_of_scroll_rows, number_read) ;)

      if ((number_of_scroll_rows - number_read) > 0)
        memmove (&the_grid[real_scroll_region_beg * screen_buffer->cols],
                 &the_grid[(real_scroll_region_beg + number_read) * screen_buffer->cols],
                 (number_of_scroll_rows - number_read) * screen_buffer->cols * sizeof (CELL)) ;

      for (Nix = real_scroll_region_end - number_read + 1 ; Nix <= real_scroll_region_end ; Nix++)
        {
        row_offset = Nix * screen_buffer->cols ;
        for (Nix1 = 0 ; Nix1 < screen_buffer->cols ; Nix1++)
          the_grid[row_offset + Nix1] = screen->current_cell ;
        }

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'T': // scroll the scrolling region down n lines
      {
      int Nix, Nix1, row_offset = -1 ;
      int real_scroll_region_beg = screen->scroll_region_beg, 
          real_scroll_region_end = screen->scroll_region_end, number_of_scroll_rows = 0 ;

      if (screen->scroll_region_beg == screen->scroll_region_end)
        {
        real_scroll_region_beg = 0 ;
        real_scroll_region_end = screen_buffer->rows - 1 ;
        }

      number_of_scroll_rows = real_scroll_region_end - real_scroll_region_beg + 1 ;
      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      number_read = MIN (number_read, number_of_scroll_rows) ;

      D_ESC (g_print ("interpret_escape: CASE: Scroll buffer (%d<-->%d) =%d lines reverse %d lines\n", real_scroll_region_beg, real_scroll_region_end, number_of_scroll_rows, number_read) ;)

      if ((number_of_scroll_rows - number_read) > 0)
        memmove (&the_grid[(real_scroll_region_beg + number_read) * screen_buffer->cols],
                 &the_grid[real_scroll_region_beg * screen_buffer->cols],
                 (number_of_scroll_rows - number_read) * screen_buffer->cols * sizeof (CELL)) ;

      for (Nix = real_scroll_region_beg ; Nix < real_scroll_region_beg + number_read ; Nix++)
        {
        row_offset = Nix * screen_buffer->cols ;
        for (Nix1 = 0 ; Nix1 < screen_buffer->cols ; Nix1++)
          the_grid[row_offset + Nix1] = screen->current_cell ;
        }

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'X': // erase a specified number of characters; do not advance the cursor
      {
      int Nix, end_col_idx, row_offset = -1 ;

      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) ;
      number_read = MIN (number_read, screen_buffer->cols - screen->current_col) ;
      end_col_idx = screen->current_col + number_read ;
      row_offset  = screen->current_row * screen_buffer->cols ;

      for (Nix = screen->current_col ; Nix < end_col_idx ; Nix++)
        the_grid[row_offset + Nix] = screen->current_cell ;

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'c': // show/hide cursor
      {
      int idx = beg_char_idx + 1 ;

      if ('?' == exp_array_index_1d (screen_buffer->bucket, char, idx++))
        for (; idx < end_char_idx ; idx++)
          if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, idx)))
            if ((number_read = read_int (screen_buffer->bucket, idx, &idx, 1)) < 2)
              screen->show_cursor_p = (0 == number_read) ;

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'd': // Set current row
      D_ESC (g_print ("interpret_escape: CASE: set current row: %d -> ", screen->current_row) ;)

      number_read = read_int (screen_buffer->bucket, beg_char_idx + 1, NULL, 1) - 1 ;
      screen->current_row = CLAMP (number_read, 0, screen_buffer->rows - 1) ;

      D_ESC (g_print ("%d\n", screen->current_row) ;)
      esc_ret = ESC_SUCCESS ;
      break ;

    case 's':
    case 'h':
    case 'l':
      {
      int idx = beg_char_idx + 1 ;

      if ('?' == exp_array_index_1d (screen_buffer->bucket, char, idx))
        {
        D_ESC (g_print ("interpret_escape: CASE: '?.%c': ", exp_array_index_1d (screen_buffer->bucket, char, end_char_idx)) ;)
        for (idx++; idx < end_char_idx ; idx++)
          if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, idx)))
            {
            number_read = read_int (screen_buffer->bucket, idx, &idx, 1) ;
            D_ESC (g_print ("%d: ", number_read) ;)

            if (25 == number_read)
              D_ESC ({)
              screen->show_cursor_p = ('h' == exp_array_index_1d (screen_buffer->bucket, char, end_char_idx)) ;
              D_ESC (g_print ("cursor now %sshown ... ", screen->show_cursor_p ? "" : "not ") ;)
              D_ESC (})

            if (47 == number_read || 48 == number_read || 1047 == number_read || 1048 == number_read || 1049 == number_read)
              {
              screen = 
              screen_buffer->current_screen = ('h' == exp_array_index_1d (screen_buffer->bucket, char, end_char_idx)) ?
                screen_buffer->alt_screen : screen_buffer->prm_screen ;
              the_grid = screen->the_grid ;
              }
            }
        D_ESC (g_print ("\n") ;)
        }
      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'm': // perdy cuh-lurrs
      {
      int idx = beg_char_idx + 1 ;

      // Cancel everything
      if (idx == end_char_idx)
        screen->current_cell = (*default_cell) ;

      for (; idx < end_char_idx ; idx++)
        D_ESC ({)
        if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, idx)))
          {
          number_read = read_int (screen_buffer->bucket, idx, &idx, 1) ;
          D_ESC (g_print ("interpret_escape: CASE: perdy cuh-lurrs: Read number %d\n", number_read) ;)
          if (number_read < 10) // bold/blinky/hidden/underscore/inverse/normal
            {
            D_ESC (g_print ("interpret_escape: CASE: perdy cuh-lurrs: special: (%s %s %s %s %s) -> ",
              screen->current_cell.special & SPSET_USCR ? "SPSET_USCR" : "",
              screen->current_cell.special & SPSET_BLNK ? "SPSET_BLNK" : "",
              screen->current_cell.special & SPSET_BOLD ? "SPSET_BOLD" : "",
              screen->current_cell.special & SPSET_INVS ? "SPSET_INVS" : "",
              screen->current_cell.special & SPSET_HIDN ? "SPSET_HIDN" : "") ;)

            if (0 == number_read)
              {
              screen->current_cell.fg_clr  = default_cell->fg_clr ;
              screen->current_cell.bg_clr  = default_cell->bg_clr ;
              screen->current_cell.special = default_cell->special ;
              }

            screen->current_cell.special |= SPECIAL_USCR == number_read ? SPSET_USCR :
                                            SPECIAL_BLNK == number_read ? SPSET_BLNK :
                                            SPECIAL_BOLD == number_read ? SPSET_BOLD :
                                            SPECIAL_INVS == number_read ? SPSET_INVS :
                                            SPECIAL_HIDN == number_read ? SPSET_HIDN : SPSET_NONE ;

            D_ESC (g_print ("(%s %s %s %s %s)\n",
              screen->current_cell.special & SPSET_USCR ? "SPSET_USCR" : "",
              screen->current_cell.special & SPSET_BLNK ? "SPSET_BLNK" : "",
              screen->current_cell.special & SPSET_BOLD ? "SPSET_BOLD" : "",
              screen->current_cell.special & SPSET_INVS ? "SPSET_INVS" : "",
              screen->current_cell.special & SPSET_HIDN ? "SPSET_HIDN" : "") ;)
            }
          else
          if (10 == number_read) // Line-drawing off
            {
            D_ESC (g_print ("interpret_escape: CASE: perdy cuh-lurrs: line_drawing off\n") ;)
            screen->can_line_draw_p = FALSE ;
            screen->current_cell.line_drawing_status = LINES_NONE ;
            }
          else
          if (11 == number_read) // Line-drawing on
            {
            D_ESC (g_print ("interpret_escape: CASE: perdy cuh-lurrs: line_drawing on\n") ;)
            screen->can_line_draw_p = TRUE ;
            if (LINES_NONE == screen->current_cell.line_drawing_status)
              screen->current_cell.line_drawing_status = LINES_OFF ;
            }

          else if (22 == number_read) screen->current_cell.special &= ~SPSET_BOLD ; // Cancel bold 
          else if (24 == number_read) screen->current_cell.special &= ~SPSET_USCR ; // Cancel underscore
          else if (25 == number_read) screen->current_cell.special &= ~SPSET_BLNK ; // Cancel blinky
          else if (27 == number_read) screen->current_cell.special &= ~SPSET_INVS ; // Cancel inverse
          else if (39 == number_read) screen->current_cell.fg_clr = default_cell->fg_clr ; // reset foreground
          else if (49 == number_read) screen->current_cell.bg_clr = default_cell->bg_clr ; // reset background
          else if (number_read >= 40 && number_read <= 47) screen->current_cell.bg_clr = number_read - 40 ; // background
          else if (number_read >= 30 && number_read <= 37) screen->current_cell.fg_clr = number_read - 30 + // foreground
            (screen->current_cell.special & SPSET_BOLD ? 8 : 0) ;                                    // fg bold offset
          }

        D_ESC ({
          if (number_read >= 30 && number_read <= 50)
            g_print ("interpret_escape: CASE: perdy cuh-lurrs: fg: %s: bg: %s\n",
              CLR_BLACK    == screen->current_cell.fg_clr ? "CLR_BLACK" :
              CLR_RED      == screen->current_cell.fg_clr ? "CLR_RED" :
              CLR_GREEN    == screen->current_cell.fg_clr ? "CLR_GREEN" :
              CLR_BROWN    == screen->current_cell.fg_clr ? "CLR_BROWN" :
              CLR_BLUE     == screen->current_cell.fg_clr ? "CLR_BLUE" :
              CLR_PURPLE   == screen->current_cell.fg_clr ? "CLR_PURPLE" :
              CLR_CYAN     == screen->current_cell.fg_clr ? "CLR_CYAN" :
              CLR_LTGRAY   == screen->current_cell.fg_clr ? "CLR_LTGRAY" :
              CLR_DKGRAY   == screen->current_cell.fg_clr ? "CLR_DKGRAY" :
              CLR_LTRED    == screen->current_cell.fg_clr ? "CLR_LTRED" :
              CLR_LTGREEN  == screen->current_cell.fg_clr ? "CLR_LTGREEN" :
              CLR_YELLOW   == screen->current_cell.fg_clr ? "CLR_YELLOW" :
              CLR_LTBLUE   == screen->current_cell.fg_clr ? "CLR_LTBLUE" :
              CLR_LTPURPLE == screen->current_cell.fg_clr ? "CLR_LTPURPLE" :
              CLR_LTCYAN   == screen->current_cell.fg_clr ? "CLR_LTCYAN" :
              CLR_WHITE    == screen->current_cell.fg_clr ? "CLR_WHITE" : "CLR_CUSTOM",

              CLR_BLACK    == screen->current_cell.bg_clr ? "CLR_BLACK" :
              CLR_RED      == screen->current_cell.bg_clr ? "CLR_RED" :
              CLR_GREEN    == screen->current_cell.bg_clr ? "CLR_GREEN" :
              CLR_BROWN    == screen->current_cell.bg_clr ? "CLR_BROWN" :
              CLR_BLUE     == screen->current_cell.bg_clr ? "CLR_BLUE" :
              CLR_PURPLE   == screen->current_cell.bg_clr ? "CLR_PURPLE" :
              CLR_CYAN     == screen->current_cell.bg_clr ? "CLR_CYAN" :
              CLR_LTGRAY   == screen->current_cell.bg_clr ? "CLR_LTGRAY" :
              CLR_DKGRAY   == screen->current_cell.bg_clr ? "CLR_DKGRAY" :
              CLR_LTRED    == screen->current_cell.bg_clr ? "CLR_LTRED" :
              CLR_LTGREEN  == screen->current_cell.bg_clr ? "CLR_LTGREEN" :
              CLR_YELLOW   == screen->current_cell.bg_clr ? "CLR_YELLOW" :
              CLR_LTBLUE   == screen->current_cell.bg_clr ? "CLR_LTBLUE" :
              CLR_LTPURPLE == screen->current_cell.bg_clr ? "CLR_LTPURPLE" :
              CLR_LTCYAN   == screen->current_cell.bg_clr ? "CLR_LTCYAN" :
              CLR_WHITE    == screen->current_cell.bg_clr ? "CLR_WHITE" : "CLR_CUSTOM") ;
        })

        D_ESC (g_print ("\n") ;)

        D_ESC (})

      esc_ret = ESC_SUCCESS ;
      break ;
      }

    case 'r': // Set scrolling region
      {
      gboolean single_number = TRUE ;
      // By default, the new range is the entire screen
      int new_region_beg = 0, new_region_end = screen_buffer->rows - 1 ;
      int idx = beg_char_idx + 1 ;

      if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, idx)))
        {
        // If we have a number ...
        number_read = read_int (screen_buffer->bucket, idx, &idx, screen_buffer->rows) - 1 ;
        number_read = CLAMP (number_read, 0, screen_buffer->rows - 1) ;
        D_ESC (g_print ("interpret_escape: CASE: setting scrolling region: Read first number: %d - bucket[idx=%d] is %c\n", number_read, idx, exp_array_index_1d (screen_buffer->bucket, char, idx)) ;)

        if (idx < end_char_idx)
          {
          idx++ ;
          if (IS_NUMERIC (exp_array_index_1d (screen_buffer->bucket, char, idx)))
            {
            // ... and we have another number, then we have the new range of rows, so use it
            new_region_beg = number_read ;
            number_read = read_int (screen_buffer->bucket, idx, NULL, screen_buffer->rows) - 1 ;
            D_ESC (g_print ("interpret_escape: CASE: setting scrolling region: Read second number: %d - bucket[idx=%d] is %c\n", number_read, idx, exp_array_index_1d (screen_buffer->bucket, char, idx)) ;)
            new_region_end = CLAMP (number_read, 0, screen_buffer->rows - 1) ;
            if (new_region_beg > new_region_end) // swap 'em if they're weird
              {
              number_read = new_region_beg ;
              new_region_beg = new_region_end ;
              new_region_end = number_read ;
              }
            single_number = FALSE ;
            }
          }
        // ... and we don't have a second number, then the one number marks the end of the new range of rows
        if (single_number)
          new_region_end = number_read ;
        }

      D_ESC (g_print ("interpret_escape: CASE: setting scrolling region from [%d ; %d] to [%d ; %d]\n", screen->scroll_region_beg, screen->scroll_region_end, new_region_beg, new_region_end) ;)

      screen->scroll_region_beg = new_region_beg ;
      screen->scroll_region_end = new_region_end ;

      esc_ret = ESC_SUCCESS ;
      break ;
      }
    }

  if (esc_ret != ESC_SUCCESS)
    (*pidx) = beg_char_idx - 1 ;
  D_ESC(else g_print ("interpret_escape: Exiting successfully. current (r,c) = (%d,%d)\n\n", screen->current_row, screen->current_col) ;)

  return esc_ret ;
  }

static gboolean find_ending_char (EXP_ARRAY *bucket, int idx, int *p_end_idx)
  {
  if (idx >= bucket->icUsed)
    return FALSE ;

  // non-titlebar string
  if ('[' == exp_array_index_1d (bucket, char, idx))
    {
    while (++idx < bucket->icUsed)
      if (!((';' == exp_array_index_1d (bucket, char, idx)) || 
             IS_NUMERIC (exp_array_index_1d (bucket, char, idx)) ||
             ('?' == exp_array_index_1d (bucket, char, idx))))
        break ;

    if (idx == bucket->icUsed) return FALSE ;
    }
  else
  // titlebar string
  if (']' == exp_array_index_1d (bucket, char, idx))
    {
    while (++idx < bucket->icUsed)
      if (007 == exp_array_index_1d (bucket, char, idx))
        break ;

    if (idx == bucket->icUsed) return FALSE ;
    }
  else
  if ('(' == exp_array_index_1d (bucket, char, idx) ||
      ')' == exp_array_index_1d (bucket, char, idx))
    {
    if (idx + 1 < bucket->icUsed)
      {
      if ('A' == exp_array_index_1d (bucket, char, idx + 1) ||
          'B' == exp_array_index_1d (bucket, char, idx + 1) ||
          '0' == exp_array_index_1d (bucket, char, idx + 1))
        {
        char c = exp_array_index_1d (bucket, char, idx + 1) ;
        exp_array_index_1d (bucket, char, idx + 1) = exp_array_index_1d (bucket, char, idx) ;
        exp_array_index_1d (bucket, char, idx) = c ;
        }
      }
    idx++ ;
    }

  (*p_end_idx) = idx ;

  return TRUE ;
  }

static int read_int (EXP_ARRAY *bucket, int idx, int *p_new_idx, int default_value)
  {
  int the_int = default_value ;
  char old_c ;
  int idx_beg = idx ;

  D_ESC_READ_INT (g_print ("read_int: Entering with %c\n", exp_array_index_1d (bucket, char, idx)) ;)

  for (; idx < bucket->icUsed; idx++)
    if (exp_array_index_1d (bucket, char, idx) < '0' ||
        exp_array_index_1d (bucket, char, idx) > '9')
      break ;

  if (NULL != p_new_idx)
    (*p_new_idx) = idx ;

  old_c = exp_array_index_1d (bucket, char, idx) ;
  exp_array_index_1d (bucket, char, idx) = 0 ;

  D_ESC_READ_INT({
    if (0 == exp_array_index_1d (bucket, char, idx_beg))
      g_print ("read_int: Couldn't find anything\n") ;
  })

  // If no integer values were actually found, return the default value  
  the_int = 
    (0 == exp_array_index_1d (bucket, char, idx_beg)) ? default_value : atoi (&exp_array_index_1d (bucket, char, idx_beg)) ;
  exp_array_index_1d (bucket, char, idx) = old_c ;

  return the_int ;
  }
