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
#include <string.h>
#include <glib.h>
#include "exp_array.h"

static void exp_array_vinsert_vals (EXP_ARRAY *exp_array, void *data, int icElements, int iDimension, va_list va) ;
static void exp_array_vremove_vals (EXP_ARRAY *exp_array, int icDimPairs, va_list va) ;
static void exp_array_dump_priv (EXP_ARRAY *exp_array, FILE *pfile, int icIndent, gboolean bReverseVideo) ;
// static void exp_array_empty (EXP_ARRAY *exp_array) ;
static void exp_array_insert_vals_flat (EXP_ARRAY *exp_array, void *data, int icElements, int idx) ;
static int exp_array_1d_find_priv (EXP_ARRAY *exp_array, void *element, GCompareFunc fn) ;

EXP_ARRAY *exp_array_new (int cbElementSize, int icDimensions)
  {
  EXP_ARRAY *exp_array = NULL ;

  exp_array = g_malloc0 (sizeof (EXP_ARRAY)) ;

  if (NULL == exp_array) return exp_array ;

  exp_array->icDimensions = icDimensions ;
  exp_array->cbSize = cbElementSize ;
  exp_array->icUsed =
  exp_array->icAvail = 0 ;
  exp_array->data = NULL ;

  return exp_array ;
  }

// This function always returns NULL
EXP_ARRAY *exp_array_free (EXP_ARRAY *exp_array)
  {
  int Nix ;

  if (NULL == exp_array) return NULL ;

  if (exp_array->icDimensions > 1)
    for (Nix = 0 ; Nix < exp_array->icUsed ; Nix++)
      exp_array_free (((EXP_ARRAY **)(exp_array->data))[Nix]) ;

  if (NULL != exp_array->data)
    g_free (exp_array->data) ;
  g_free (exp_array) ;

  return NULL ;
  }

// iDimension is the 1-based index of the dimension where the elements are to be inserted.
// Dimensions are indexed as follows:
// exp_array[1][2][3]...[n]
// Thus, for a 2-dimensional array, iDimension = 1 will add one member of data to each row at a
// specified column, whereas iDimension = 2 will add one member of data to each column at a specified row
// The ... arguments are tuples (index, bInsert). There are as many tuples as there are dimensions. Each
// index specifies where in the array the insertion is to start, and bInsert specifies whether to force
// creation of new arrays (as opposed to inserting into existing arrays, where available). Thus, in the last
// tuple, the value of bInsert is really irrelevant, because it is assumed to be TRUE - after all, the name of
// the function /is/ "insert_vals" ...
void exp_array_insert_vals (EXP_ARRAY *exp_array, void *data, int icElements, int iDimension, ...)
  {
  va_list va ;

  if (NULL == exp_array) return ;

  if (icElements <= 0) return ;

  va_start (va, iDimension) ;

  iDimension = CLAMP (iDimension, 1, exp_array->icDimensions) ;

  exp_array_vinsert_vals (exp_array, data, icElements, iDimension, va) ;

  va_end (va) ;
  }

int exp_array_1d_insert_val_sorted (EXP_ARRAY *exp_array, void *data, GCompareFunc fn, gboolean bAllowDupes)
  {
  int idx = 0 ;
  if (NULL == exp_array || NULL == data || NULL == fn) return -1 ;

  if (0 == exp_array->icUsed)
    exp_array_insert_vals_flat (exp_array, data, 1, -1) ;
  else
    {
    if (-1 == (idx = exp_array_1d_find_priv (exp_array, data, fn)))
      return -1 ;
    if (bAllowDupes || idx == exp_array->icUsed)
      exp_array_insert_vals_flat (exp_array, data, 1, idx) ;
    else
    if ((*fn) (data, &(((char *)(exp_array->data))[idx * exp_array->cbSize])) != 0)
      exp_array_insert_vals_flat (exp_array, data, 1, idx) ;
    else
      idx = -1 ;
    }
  return idx ;
  }

int exp_array_1d_find (EXP_ARRAY *exp_array, void *element, GCompareFunc fn, gboolean bClosest)
  {
  int idx = -1 ;

  if (0 == exp_array->icUsed) return -1 ;

  idx = exp_array_1d_find_priv (exp_array, element, fn) ;

  if (bClosest) return idx ;

  if (exp_array->icUsed == idx || idx < 0) return -1 ;

  return ((0 == (*fn) (element, ((void *)(&(((char *)(exp_array->data))[idx * exp_array->cbSize])))))) ? idx : -1 ;
  }

int compare_ints (void *p1, void *p2)
  {
  return (((*((int *)p1)) < (*((int *)p2))) ? -1 :
          ((*((int *)p1)) > (*((int *)p2))) ?  1 : 0) ;
  }

static int exp_array_1d_find_priv (EXP_ARRAY *exp_array, void *element, GCompareFunc fn)
  {
  int upper, lower, Nix1 ;
  void *ar_element = NULL ;

  if (((*fn) (element, (void *)(&(((char *)exp_array->data)[0])))) <= 0)
    return 0 ;
  else
  if (((*fn) (element, (void *)(&(((char *)exp_array->data)[(exp_array->icUsed - 1) * exp_array->cbSize])))) > 0)
    return exp_array->icUsed ;
  else
    {
    lower = 0 ;
    upper = exp_array->icUsed - 1 ;
    Nix1 = exp_array->icUsed >> 1 ;
    do
      {
      ar_element = (void *)(&(((char *)exp_array->data)[Nix1 * exp_array->cbSize])) ;
      if ((*fn) (element, ar_element) > 0)
        lower = Nix1 ;
      else
        upper = Nix1 ;
      Nix1 = (upper + lower) >> 1 ;
      }
    while (lower + 1 < upper) ;

    if ((((*fn) (element, (void *)(&(((char *)exp_array->data)[lower * exp_array->cbSize])))) > 0)) lower++ ;
    return lower ;
    }
  }

static void exp_array_vinsert_vals (EXP_ARRAY *exp_array, void *data, int icElements, int iDimension, va_list va)
  {
  int idx = -1 ;
  gboolean bInsert = FALSE ;

  idx = va_arg (va, int) ;
  if (-1 == idx) idx = exp_array->icUsed ;
  idx = CLAMP (idx, 0, exp_array->icUsed) ;

  bInsert = va_arg (va, gboolean) ;

  if (iDimension > 1)
    {
    if (idx == exp_array->icUsed || bInsert)
      {
      exp_array_insert_vals_flat (exp_array, NULL, 1, idx) ;
      exp_array_index_1d (exp_array, EXP_ARRAY *, idx) = exp_array_new (exp_array->cbSize, exp_array->icDimensions - 1) ;
      }
    exp_array_vinsert_vals (exp_array_index_1d (exp_array, EXP_ARRAY *, idx), data, icElements, iDimension - 1, va) ;
    }
  else
  if (1 == iDimension)
    {
    if (exp_array->icDimensions > 1)
      {
      int Nix ;
      va_list vaNew ;
      int old_used = -1 ;

      if (bInsert)
        {
        exp_array_insert_vals_flat (exp_array, NULL, icElements, idx) ;
        for (Nix = idx ; Nix < idx + icElements ; Nix++)
          if (NULL == exp_array_index_1d (exp_array, EXP_ARRAY *, Nix))
            exp_array_index_1d (exp_array, EXP_ARRAY *, Nix) = exp_array_new (exp_array->cbSize, exp_array->icDimensions - 1) ;
        }
      else
        {
        if (idx + icElements > (old_used = exp_array->icUsed))
          {
          exp_array_insert_vals_flat (exp_array, NULL, idx + icElements - exp_array->icUsed, exp_array->icUsed) ;
          for (Nix = old_used ; Nix < exp_array->icUsed ; Nix++)
            if (NULL == exp_array_index_1d (exp_array, EXP_ARRAY *, Nix))
              exp_array_index_1d (exp_array, EXP_ARRAY *, Nix) = exp_array_new (exp_array->cbSize, exp_array->icDimensions - 1) ;
          }
        }
      for (Nix = 0 ; Nix < icElements ; Nix++)
        {
        va_copy (vaNew, va) ;
        exp_array_vinsert_vals (exp_array_index_1d (exp_array, EXP_ARRAY *, Nix + idx), 
          NULL == data ? NULL : data + Nix * exp_array->cbSize, 1, exp_array->icDimensions - 1, vaNew) ;
        va_end (vaNew) ;
        }
      }
    else
      exp_array_insert_vals_flat (exp_array, data, icElements, idx) ;
    }
  }

static void exp_array_insert_vals_flat (EXP_ARRAY *exp_array, void *data, int icElements, int idx)
  {
  int icNeeded = -1 ;
  int cbSize = (exp_array->icDimensions > 1 ? sizeof (EXP_ARRAY *) : exp_array->cbSize) ;

  if (-1 == idx) idx = exp_array->icUsed ;
  idx = CLAMP (idx, 0, exp_array->icUsed) ;

  if ((icNeeded = icElements + exp_array->icUsed) > exp_array->icAvail)
    exp_array->data = g_realloc (exp_array->data,
      (exp_array->icAvail = MAX ((exp_array->icAvail << 1) + 1, icNeeded)) * cbSize) ;

  if (idx < exp_array->icUsed)
    memmove (
      &(((char *)(exp_array->data))[(idx + icElements) * cbSize]),
      &(((char *)(exp_array->data))[idx * cbSize]),
      (exp_array->icUsed - idx) * cbSize) ;

  if (NULL != data)
    memcpy (exp_array->data + idx * cbSize, data, icElements * cbSize) ;
  else
    memset (exp_array->data + idx * cbSize, 0, icElements * cbSize) ;
  exp_array->icUsed += icElements ;
  }

void exp_array_remove_vals (EXP_ARRAY *exp_array, int icDimPairs, ...)
  {
  va_list va ;

  if (NULL == exp_array || 0 == icDimPairs) return ;

  va_start (va, icDimPairs) ;

  exp_array_vremove_vals (exp_array, icDimPairs, va) ;
  }

static void exp_array_vremove_vals (EXP_ARRAY *exp_array, int icDimPairs, va_list va)
  {
  int idx = -1, Nix, Nix1 ;
  EXP_ARRAY *child = NULL ;
  gboolean bEnd = FALSE ;
  int icElements = -1 ;
  va_list xa ;

  idx = va_arg (va, int) ;
  icElements = va_arg (va, int) ;
  idx = CLAMP (idx, 0, exp_array->icUsed - 1) ;
  icElements = MIN (icElements, exp_array->icUsed - idx) ;

  if (0 == icElements) return ;

  if (icDimPairs > 1)
    {
    for (Nix = idx ; Nix < idx + icElements ; Nix++)
      {
      va_copy (xa, va) ;
      exp_array_vremove_vals (((EXP_ARRAY **)(exp_array->data))[Nix], icDimPairs - 1, xa) ;
      }

// Clean up - arrays that have emptied as a result of the recursive remove must in turn be removed.
    for (Nix = idx ; Nix < exp_array->icUsed ;)
      if (0 == ((EXP_ARRAY **)(exp_array->data))[Nix]->icUsed)
        {
        Nix1 = Nix ;
        while (0 == ((EXP_ARRAY **)(exp_array->data))[Nix1]->icUsed)
          {
          Nix1++ ;
          if ((bEnd = (Nix1 == exp_array->icUsed))) break ;
          }
        if (bEnd) break ;
        bEnd = FALSE ;
        while ((child = ((EXP_ARRAY **)(exp_array->data))[Nix1])->icUsed != 0)
          {
          ((EXP_ARRAY **)(exp_array->data))[Nix1] = ((EXP_ARRAY **)(exp_array->data))[Nix] ;
          ((EXP_ARRAY **)(exp_array->data))[Nix] = child ;
          Nix++ ; Nix1++ ;
          if (Nix1 == exp_array->icUsed)
            {
            bEnd = TRUE ;
            break ;
            }
          }
        if (bEnd) break ;
        }
      else
        Nix++ ;
    exp_array->icUsed = Nix ;
    }
  else
  if (exp_array->icUsed > 0)
    {
    if (idx + icElements < exp_array->icUsed)
      memmove (exp_array->data + (      idx          * exp_array->cbSize),
               exp_array->data + ((idx + icElements) * exp_array->cbSize),
              (exp_array->icUsed - idx - icElements) * exp_array->cbSize) ;
    exp_array->icUsed -= icElements ;
    }

  va_end (va) ;
  }

EXP_ARRAY *exp_array_copy (EXP_ARRAY *exp_array)
  {
  EXP_ARRAY *ret = NULL ;

  if (NULL == exp_array) return NULL ;

  ret = exp_array_new (exp_array->cbSize, exp_array->icDimensions) ;

  if (exp_array->icDimensions > 1)
    {
    int Nix ;

    ret->data = g_malloc0 ((ret->icUsed = ret->icAvail = exp_array->icUsed) * sizeof (EXP_ARRAY *)) ;
    for (Nix = 0 ; Nix < exp_array->icUsed ; Nix++)
      ((EXP_ARRAY **)(ret->data))[Nix] = exp_array_copy (((EXP_ARRAY **)(exp_array->data))[Nix]) ;
    }
  else
    {
    ret->data = g_malloc0 ((ret->icUsed = ret->icAvail = exp_array->icUsed) * exp_array->cbSize) ;
    memcpy (ret->data, exp_array->data, exp_array->icUsed * exp_array->cbSize) ;
    }

  return ret ;
  }
/*
static void exp_array_empty (EXP_ARRAY *exp_array)
  {
  int Nix ;

  if (exp_array->icDimensions > 1)
    for (Nix = 0 ; Nix < exp_array->icUsed ; Nix++)
      exp_array_empty (((EXP_ARRAY **)(exp_array->data))[Nix]) ;
  exp_array->icUsed = 0 ;
  }
*/
void exp_array_dump (EXP_ARRAY *exp_array, FILE *pfile, int icIndent)
  {
  fprintf (pfile, "%*sexp_array_dump (0x%08X):\n", icIndent, "", (int)exp_array) ;
  exp_array_dump_priv (exp_array, pfile, icIndent, FALSE) ;
  }

static void exp_array_dump_priv (EXP_ARRAY *exp_array, FILE *pfile, int icIndent, gboolean bReverseVideo)
  {
  int Nix ;
  char *pszReverseOn = bReverseVideo ? "\033[7m" : "" ;
  char *pszReverseOff = bReverseVideo ? "\033[0m" : "" ;

  fprintf (pfile, "%*s%s->icUsed = %d%s\n", icIndent, "", pszReverseOn, exp_array->icUsed, pszReverseOff) ;
  fprintf (pfile, "%*s%s->icAvail = %d%s\n", icIndent, "", pszReverseOn, exp_array->icAvail, pszReverseOff) ;
  fprintf (pfile, "%*s%s->cbSize = %d%s\n", icIndent, "", pszReverseOn, exp_array->cbSize, pszReverseOff) ;
  fprintf (pfile, "%*s%s->icDimensions = %d%s\n", icIndent, "", pszReverseOn, exp_array->icDimensions, pszReverseOff) ;
  fprintf (pfile, "%*s%s->data (0x%08X):%s\n", icIndent, "", pszReverseOn, (int)(exp_array->data), pszReverseOff) ;
  if (1 == exp_array->icDimensions)
    print_hex_bytes ((char *)(exp_array->data), exp_array->icAvail * exp_array->cbSize, exp_array->icUsed * exp_array->cbSize, 8, pfile, icIndent) ;
  else
    {
    for (Nix = 0 ; Nix < exp_array->icUsed ; Nix++)
      exp_array_dump_priv (exp_array_index_1d (exp_array, EXP_ARRAY *, Nix), pfile, icIndent + 2, TRUE) ;
    for (; Nix < exp_array->icAvail ; Nix++)
      if (NULL == exp_array_index_1d (exp_array, EXP_ARRAY *, Nix))
        fprintf (pfile, "NULL\n") ;
      else
        exp_array_dump_priv (exp_array_index_1d (exp_array, EXP_ARRAY *, Nix), pfile, icIndent + 2, FALSE) ;
    }
  }

void print_hex_bytes (char *bytes, int icBytes, int icInitBytes, int icCols, FILE *pfile, int icIndent)
  {
  int Nix ;

  if (NULL == bytes) return ;

  if (icBytes > 0)
    fprintf (pfile, "%*s", icIndent, "") ;
  for (Nix = 0 ; Nix < icBytes ; Nix++)
    {
    fprintf (pfile, "%s%02X%s ",
      Nix < icInitBytes ? "\033[7m" : "",
      (unsigned char)(bytes[Nix]),
      Nix < icInitBytes ? "\033[0m" : "") ;
    if (Nix % icCols == icCols - 1)
      fprintf (pfile, "\n%*s", (Nix == icBytes - 1) ? 0 : icIndent, "") ;
    }
  if (Nix % icCols)
    fprintf (pfile, "\n") ;
  }

guint exp_array_crc32 (EXP_ARRAY *exp_array)
  {
  guint cb = 0 ;
  guint Nix ;
  guint val = 0 ;
  guint ret = 0 ;

  if (NULL == exp_array) return 0 ;

  cb = exp_array->icUsed * exp_array->cbSize ;

  for (Nix = 0 ; Nix < cb ; Nix++)
    {
    val = (guint)(((unsigned char *)(exp_array->data))[Nix] & 0xFF) ;
    ret = (0xFFFFFFFF - ret < val) ? (val - 0xFFFFFFFF - ret - 1) : (ret + val) ;
    }

  return ret ;
  }
