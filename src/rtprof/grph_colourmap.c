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

#include "grph_common.h"

#define MAX_CTABLE_ENTRIES  1024

static vec4_t colourTable[ MAX_CTABLE_ENTRIES ];

/*
===============
buildLerpCTable

Build a colour table by interpolating between a
number of discrete colours
===============
*/
void buildLerpCTable( vec4_t *colours, int numColours )
{
  int     i;
  int     numRanges = numColours - 1;
  int     range;
  float   rangeSize = (float)MAX_CTABLE_ENTRIES / (float)numRanges;
  float   scale;
  vec4_t  from, to;
  
  if( numColours < 2 )
    return;

  for( i = 0; i < MAX_CTABLE_ENTRIES; i++ )
  {
    range = (int)floor( (float)i / rangeSize );
    Vector4Copy( colours[ range ], from );
    Vector4Copy( colours[ range + 1 ], to );

    scale = (float)( i % (int)rangeSize ) / rangeSize;

    if( scale < 0.0f )
      scale = 0.0f;
    else if( scale > 1.0f )
      scale = 1.0f;

    colourTable[ i ][ 0 ] = ( ( 1.0f - scale ) * from[ 0 ] + scale * to[ 0 ] );
    colourTable[ i ][ 1 ] = ( ( 1.0f - scale ) * from[ 1 ] + scale * to[ 1 ] );
    colourTable[ i ][ 2 ] = ( ( 1.0f - scale ) * from[ 2 ] + scale * to[ 2 ] );
    colourTable[ i ][ 3 ] = ( ( 1.0f - scale ) * from[ 3 ] + scale * to[ 3 ] );
  }
}

/*
===============
lookupColour

Lookup a colour in the colour table
===============
*/
void lookupColour( float scale, vec4_t colour )
{
  if( scale < 0.0f )
    scale = 0.0f;
  else if( scale > 1.0f ) 
    scale = 1.0f;

  Vector4Copy( colourTable[ (int)( scale * (float)( MAX_CTABLE_ENTRIES - 1 ) ) ], colour );
}
