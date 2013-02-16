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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bfd.h>

#include "adt_symbol.h"

static symbolNode_t *buckets[ MAX_SYMBOL_BUCKETS ];

/*
===============
addToSymbolBucket

Add a new graphNode_t to a bucket chain
===============
*/
static symbolNode_t *addToSymbolBucket( symbolNode_t *bucket,
                                        void *symbol, char *textSymbol )
{
  symbolNode_t *head = bucket;

  if( bucket )
  {
    //already elements in the list
    while( bucket->next )
      bucket = bucket->next;
      
    bucket->next = (symbolNode_t *)malloc( sizeof( symbolNode_t ) );
    bucket = bucket->next;
    
    memset( bucket, 0, sizeof( symbolNode_t ) );
    bucket->symbol = symbol;
    strncpy( bucket->textSymbol, textSymbol, MAX_SYMBOL_TEXT );
    bucket->next = NULL;
    
    return head;
  }
  else
  {
    //list is empty
    bucket = (symbolNode_t *)malloc( sizeof( symbolNode_t ) );
    
    memset( bucket, 0, sizeof( symbolNode_t ) );
    bucket->symbol = symbol;
    strncpy( bucket->textSymbol, textSymbol, MAX_SYMBOL_TEXT );
    bucket->next = NULL;
    
    return bucket;
  }
}

/*
===============
findChainlink

Return a graphNode_t based on 
===============
*/
static symbolNode_t *findSymbolInChain( symbolNode_t *bucket, void *symbol )
{
  symbolNode_t *p = bucket;

  while( p )
  {
    if( p->symbol == symbol )
      return p;
      
    p = p->next;
  }
  
  return NULL;
}


/*
===============
lookupSymbol

Lookup a symbol and return a text symbol
===============
*/
char *lookupSymbol( void *symbol )
{
  symbolNode_t  *node;
  int           index = (int)( (long)symbol % MAX_SYMBOL_BUCKETS );
  
  node = findSymbolInChain( buckets[ index ], symbol );

  if( node )
    return node->textSymbol;
  else
    return NULL;
}


/*
===============
addSymbol

Add a symbol to the symbol table
===============
*/
void addSymbol( void *symbol, char *textSymbol )
{
  int index = (int)( (long)symbol % MAX_SYMBOL_BUCKETS );
  
  buckets[ index ] = addToSymbolBucket( buckets[ index ], symbol, textSymbol );
}


//bfd returns extra duplicate symbols with gcc2_compiled. as the
//symbol name. I don't know why this is, so for the time being
//this hack is employed.
#define BFD_HACK    "gcc2_compiled."

//FIXME: hard wired for pc-linux
#define BFD_TARGET  "i686-pc-linux-gnu"

/*
===============
resolveSymbols

Open a binary file and get symbols
===============
*/
void resolveSymbols( char *binFile )
{
  bfd           *file;
  long          symcount;
  PTR           minisyms;
  unsigned int  size;
  asymbol       *store, *sym;
  bfd_byte      *from, *fromend;
  symbol_info   syminfo;

  if( !bfd_set_default_target( BFD_TARGET ) )
  {
    fprintf( stderr, "rtprof: unable to set default target\n" );
    return;
  }
  
  if( ( file = bfd_openr( binFile, NULL ) ) != NULL )
  {
    //hmm?
    if( !bfd_check_format_matches( file, bfd_object, NULL ) )
    {
      fprintf( stderr, "rtprof: bfd format doesn't match\n" );
      return;
    }
      
    //no symbols
    if( !( bfd_get_file_flags( file ) & HAS_SYMS ) )
    {
      fprintf( stderr, "rtprof: file has no symbols\n" );
      return;
    }

    symcount = bfd_read_minisymbols( file, 0, &minisyms, &size );
    
    //something broke FIXME: be more verbose
    if( symcount < 0 )
    {
      fprintf( stderr, "rtprof: unable to resolve symbols\n" );
      return;
    }

    //no symbols
    if( symcount == 0 )
    {
      fprintf( stderr, "rtprof: symcount = 0\n" );
      return;
    }
     
    if( ( store = bfd_make_empty_symbol( file ) ) == NULL )
    {
      fprintf( stderr, "rtprof: bfd_make_empty_symbol\n" );
      return;
    }
    
    from = (bfd_byte *)minisyms;    
    fromend = from + symcount * size;

    for( ; from < fromend; from += size )
    { 
      if( ( sym = bfd_minisymbol_to_symbol( file, 0, from, store ) ) == NULL )
      {
        fprintf( stderr, "rtprof: bfd_minisymbol_to_symbol\n" );
        return;
      }
         
      bfd_get_symbol_info( file, sym, &syminfo );
      
      if( syminfo.value && strlen( syminfo.name ) && strcmp( syminfo.name, BFD_HACK ) )
      {
        addSymbol( (void *)syminfo.value, (char *)syminfo.name );
        /*printf( "%p \"%s\" \"%s\"\n", syminfo.value, syminfo.name, lookupSymbol( (void *)syminfo.value ) );*/
      }
    }    
    
    bfd_close( file );
  }
  else
    fprintf( stderr, "rtprof: unable to open file %s\n", binFile );
}


/*
===============
initSymbolTable

Initialise some stuff for the symbol table
===============
*/
void initSymbolTable( void )
{
  int i;

  for( i = 0; i < MAX_SYMBOL_BUCKETS; i++ )
    buckets[ i ] = NULL;

  bfd_init( );
}

/*
===============
shutdownSymbolTable

Shutdown symbol table
===============
*/
void shutdownSymbolTable( void )
{
  int           i;
  symbolNode_t  *p, *q;

  for( i = 0; i < MAX_SYMBOL_BUCKETS; i++ )
  {
    p = buckets[ i ];
    
    while( p )
    {
      q = p->next;
      
      free( p );

      p = q;
    }
  }
}
