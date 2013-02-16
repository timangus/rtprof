/*
 * Copyright (C) 2003 Tim Angus
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#define MAX_SYMBOL_BUCKETS  1024
#define MAX_SYMBOL_TEXT     256

typedef struct symbolNode_s
{
  void                *symbol;
  char                textSymbol[ MAX_SYMBOL_TEXT ];
  
  //needed for hashtable chains
  struct symbolNode_s  *next;
} symbolNode_t;

void          resolveSymbols( char *binFile );
void          addSymbol( void *symbol, char *textSymbol );
char          *lookupSymbol( void *symbol );

void          initSymbolTable( void );
void          shutdownSymbolTable( void );
  
#endif
