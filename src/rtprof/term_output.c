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
#include <string.h>
#include <stdlib.h>

#include "term_output.h"
#include "adt_graph.h"
#include "adt_symbol.h"

/*
===============
printPadded

Print a string padded by spaces
===============
*/
static void printPadded( char *s, int n )
{
  int d = n - strlen( s );

  if( d < 0 )
    d = 0;

  printf( "%s", s );
  printSpaces( d );
}


/*
===============
dotOutput

Write a graphviz dot file
===============
*/
void dotOutput( char *filename, graph_t *g )
{
  graphEdge_t **p;
  int         n, i, j;
  FILE        *f;

  if( !strcmp( filename, "-" ) )
    f = stdout;
  else
    f = fopen( filename, "w" );

  p = listEdges( &n, g );

  fprintf( f, "digraph callgraph\n{\n" );
  
  for( i = 0; i < n; i++ )
  {
    if( p[ i ]->from->textSymbol )
      fprintf( f, "\t\"%s\" -> ", p[ i ]->from->textSymbol );

    if( p[ i ]->to->textSymbol )
      fprintf( f, "\"%s\";\n", p[ i ]->to->textSymbol );
  }

  fprintf( f, "}\n" );

  fclose( f );
  free( p );
}

/*
===============
outputHack

Print output
===============
*/
/*void *outputHack( void *arg )
{
  int         i, n;
  graphNode_t **p;
  char        *t;
  char        buffer[ 64 ];
  
  while( 1 )
  {
    usleep( 100000 );

    p = listNodes( SF_CALLS, &n );
    
    system( "clear" );

    printPadded( "calls", 10 );
    printPadded( "total", 10 );
    printPadded( "local", 10 );
    printPadded( "function", 10 );
    printf( "\n" );

    for( i = n - 1; i >= 0; i-- )
    {
      sprintf( buffer, "%d", p[ i ]->calls );
      printPadded( buffer, 10 );
      
      sprintf( buffer, "%.4f", 
          p[ i ]->totalTime.tv_sec + ( p[ i ]->totalTime.tv_usec / 1000000.0 ) );
      printPadded( buffer, 10 );
      
      sprintf( buffer, "%.4f", 
          p[ i ]->localTime.tv_sec + ( p[ i ]->localTime.tv_usec / 1000000.0 ) );
      printPadded( buffer, 10 );
        
      if( ( t = lookupSymbol( p[ i ]->symbol ) ) )
        printf( "%s\n", t );
      else
        printf( "%p\n", p[ i ]->symbol );
    }
  }
  
  free( p );
}*/
