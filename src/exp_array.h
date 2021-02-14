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

#ifndef _EXP_ARRAY_H_
#define _EXP_ARRAY_H_

#include <stdio.h>
#include <glib.h>

typedef struct _EXP_ARRAY EXP_ARRAY ;

struct _EXP_ARRAY
  {
  void *data ;
  int icUsed ;
  int icAvail ;
  int cbSize ;
  int icDimensions ;
  } ;

EXP_ARRAY *exp_array_new (int cbElementSize, int icDimensions) ;
EXP_ARRAY *exp_array_copy (EXP_ARRAY *exp_array) ;
EXP_ARRAY *exp_array_free (EXP_ARRAY *exp_array) ;
void exp_array_insert_vals (EXP_ARRAY *exp_array, void *data, int icElements, int iDimension, ...) ;
void exp_array_remove_vals (EXP_ARRAY *exp_array, int icDimPairs, ...) ;
void exp_array_dump (EXP_ARRAY *exp_array, FILE *pfile, int icIndent) ;
void print_hex_bytes (char *bytes, int icBytes, int icInitBytes, int icCols, FILE *pfile, int icIndent) ;
guint exp_array_crc32 (EXP_ARRAY *exp_array) ;
// 1d functions like binary search, sorted array
int exp_array_1d_insert_val_sorted (EXP_ARRAY *exp_array, void *data, GCompareFunc fn, gboolean bAllowDupes) ;
int exp_array_1d_find (EXP_ARRAY *exp_array, void *element, GCompareFunc fn, gboolean bClosest) ;
// Assorted comparison functions
int compare_ints (void *p1, void *p2) ;
#define exp_array_1d_insert_vals(a,d,c,i) (exp_array_insert_vals ((a),(d),(c),1,(i),TRUE))
//#define exp_array_index_1d(a,t,i) (((t*) (a)->data) [(i)])
//#define exp_array_index_2d(a,t,r,c) (((t*) (exp_array_index_1d((a),EXP_ARRAY *,(r)))->data) [(c)])
#define exp_array_index_1d(a,t,i) (((t*) (a)->data) [((-1 == (i)) ? (((a)->icUsed) - 1) : (i))])
#define exp_array_index_2d(a,t,r,c) (((t*) (exp_array_index_1d((a),EXP_ARRAY *,(r)))->data) [((-1 == (c)) ? (((exp_array_index_1d((a),EXP_ARRAY *,(r)))->icUsed) - 1) : (c))])

#endif /* _EXP_ARRAY_H_ */
