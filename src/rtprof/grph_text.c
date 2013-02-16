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

#include <stdarg.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glut.h>

#include "grph_text.h"
#include "com_common.h"

static GLuint base;

/*
===============
shutdownFont

Unload the font
===============
*/
void shutdownFont( GLvoid )
{
  glDeleteLists( base, 96 );
  return;
}

/*
===============
initFont

Initialise a font
===============
*/
GLFontError_t initFont( char *fontString )
{
  Display     *dpy; 
  XFontStruct *fontInfo;

  base = glGenLists( 96 );
  dpy = XOpenDisplay( NULL );

  fontInfo = XLoadQueryFont( dpy, fontString );

  //font not found
  if( fontInfo == NULL )
  {
    //this font should exist everywhere
    fontInfo = XLoadQueryFont( dpy, "-misc-fixed-*-*-*-*-*-*-*-*-*-*-*-*" );
    
    if( fontInfo == NULL )
     return GLF_FONTNOTFOUND;
  }

  glXUseXFont( fontInfo->fid, 32, 96, base );

  XFreeFont( dpy, fontInfo );
  XCloseDisplay( dpy );

  return GLF_NONE;
}

/*
===============
glPrintf

Print some text
===============
*/
void glPrintf( const char *fmt, ... )
{
  char    text[ 256 ]; 
  va_list ap;
  int     i, len;

  if( fmt == NULL )
    return;

  va_start( ap, fmt );
  
  vsprintf( text, fmt, ap );
  va_end( ap );

  len = strlen( text );

  for( i = 0; i < len; i++ )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, text[ i ] );

  //glx call list based approach
/*  glPushAttrib( GL_LIST_BIT );

  glListBase( base - 32 );

  glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );

  glPopAttrib( );*/
}
