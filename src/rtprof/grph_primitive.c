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
#include <GL/gl.h>
#include <GL/glu.h>

#include "grph_common.h"
#include "grph_primitive.h"
#include "grph_vector.h"
#include "grph_text.h"
#include "grph_colourmap.h"

#define NODE_SUBDIV  12
#define EDGE_SUBDIV  12
#define LOD_DIST     30.0f

#define TEXT_PADDING  0.2f
#define TEXT_CULL_DIST  10.0f

static vec4_t specular = { 1.0f, 1.0f, 1.0f, 1.0f };
static float  shininess[ 1 ] = { 128.0f };

#define SHELL_SIZE  0.05f
#define SHELL_ALPHA 0.5f

/*
===============
addNode

Add a graph node to the scene
===============
*/
void addNode( vec3_t origin, float scale, float cScale, float aScale,
              char *name, vec3_t tc, boolean h, vec3_t hc )
{
  GLUquadricObj *q;
  vec3_t        textOrigin;
  float         size;
  float         alpha = scaleToAlpha( aScale );
  vec4_t        diffuse;
  float         distance = Distance( camera.origin, origin );
  int           subDiv;
  vec4_t        colour;

  if( scale > 1.0f )
    scale = 1.0f;

  size = nodeScaleToSize( scale );
  
  subDiv = (int)( ( ( LOD_DIST - distance ) / LOD_DIST ) *
                  (float)NODE_SUBDIV );

  if( subDiv < 3 )
    subDiv = 3;

  subDiv = (int)( (float)subDiv * ( 1.0f + ( size / MAX_NODE_SIZE ) ) );
  
  glTranslatef( origin[ 0 ],
                origin[ 1 ],
                origin[ 2 ] );

  lookupColour( cScale, colour );
  glColor4f( colour[ 0 ], colour[ 1 ], colour[ 2 ], alpha );
  VectorCopy( colour, diffuse );
  diffuse[ 3 ] = 1.0f;
  
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

  q = gluNewQuadric( );
  gluSphere( q, size, subDiv, subDiv );
  gluDeleteQuadric( q );
  
  if( h )
  {
    glColor4f( hc[ 0 ], hc[ 1 ], hc[ 2 ], SHELL_ALPHA );
    
    q = gluNewQuadric( );
    gluSphere( q, size + SHELL_SIZE, subDiv, subDiv );
    gluDeleteQuadric( q );
  }
  
  //only draw text for nearby nodes
  if( distance < TEXT_CULL_DIST )
  {
    glDisable( GL_LIGHTING );
    
    glColor4f( tc[ 0 ], tc[ 1 ], tc[ 2 ], alpha );
    
    VectorScale( camera.axis[ 1 ], size + TEXT_PADDING, textOrigin );
    glRasterPos3f( textOrigin[ 0 ], textOrigin[ 1 ], textOrigin[ 2 ] );

    glPrintf( name );
    glEnable( GL_LIGHTING );
  }
}


#define ARROW_LENGTH  0.2f
#define ARROW_FLANGE  1.8f

/*
===============
addEdge

Add a graph edge to the scene
===============
*/
void addEdge( vec3_t origin, vec3_t dir, float length,
              float scale, float cScale, float aScale,
              boolean h, vec3_t hc )
{
  GLUquadricObj *q;
  vec3_t        forward, rotAxis;
  float         size;
  float         alpha = scaleToAlpha( aScale );
  vec3_t        diffuse;
  float         distance = Distance( camera.origin, origin );
  int           subDiv;
  vec4_t        colour;

  if( scale > 1.0f )
    scale = 1.0f;

  size = edgeScaleToSize( scale );
  
  subDiv = (int)( ( ( LOD_DIST - distance ) / LOD_DIST ) *
                  (float)EDGE_SUBDIV );

  if( subDiv < 3 )
    subDiv = 3;

  subDiv = (int)( (float)subDiv * ( 1.0f + ( size / MAX_NODE_SIZE ) ) );
  
  glTranslatef( origin[ 0 ],
                origin[ 1 ],
                origin[ 2 ] );

  //ick. must be a better way of doing this? (gluLookAt?)
  //FIXME: dir == forward == badness
  VectorSet( forward, 0.0f, 0.0f, 1.0f );
  CrossProduct( forward, dir, rotAxis );
  VectorNormalise( rotAxis );
  glRotatef( RAD2DEG( acos( DotProduct( forward, dir ) ) ),
             rotAxis[ 0 ], rotAxis[ 1 ], rotAxis[ 2 ] );

  lookupColour( cScale, colour );
  glColor4f( colour[ 0 ], colour[ 1 ], colour[ 2 ], alpha );
  VectorCopy( colour, diffuse );
  diffuse[ 3 ] = 1.0f;
  
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

  if( distance < LOD_DIST )
  {
    //shaft
    q = gluNewQuadric( );
    gluCylinder( q, 0, size, length - ( length * ARROW_LENGTH ), subDiv, 1 );
    gluDeleteQuadric( q );

    //flange
    glTranslatef( 0.0f, 0.0f, length - ( length * ARROW_LENGTH ) );
    
    q = gluNewQuadric( );
    gluQuadricOrientation( q, GLU_INSIDE );
    gluDisk( q, size, size * ARROW_FLANGE, subDiv, 1 );
    gluDeleteQuadric( q );
    
    //arrowhead
    q = gluNewQuadric( );
    gluCylinder( q, size * ARROW_FLANGE, 0, length * ARROW_LENGTH, subDiv, 1 );
    gluDeleteQuadric( q );
    
    if( h )
    {
      glColor4f( hc[ 0 ], hc[ 1 ], hc[ 2 ], SHELL_ALPHA );
    
      glTranslatef( 0.0f, 0.0f, -( length - ( length * ARROW_LENGTH ) ) - SHELL_SIZE );

      //shaft
      q = gluNewQuadric( );
      gluCylinder( q, 0, size + SHELL_SIZE, length - ( length * ARROW_LENGTH ), subDiv, 1 );
      gluDeleteQuadric( q );
      
      //flange
      glTranslatef( 0.0f, 0.0f, length - ( length * ARROW_LENGTH ) );
      
      q = gluNewQuadric( );
      gluQuadricOrientation( q, GLU_INSIDE );
      gluDisk( q, size + SHELL_SIZE, size * ARROW_FLANGE + SHELL_SIZE, subDiv, 1 );
      gluDeleteQuadric( q );

      //arrowhead
      q = gluNewQuadric( );
      gluCylinder( q, ( size * ARROW_FLANGE ) + SHELL_SIZE, 0,
                   ( length * ARROW_LENGTH ) + 2 * SHELL_SIZE, subDiv, 1 );
      gluDeleteQuadric( q );
    }
  }
  else
  {
    //shaft
    q = gluNewQuadric( );
    gluCylinder( q, 0, size, length, subDiv, 1 );
    gluDeleteQuadric( q );
    
    if( h )
    {
      glColor4f( hc[ 0 ], hc[ 1 ], hc[ 2 ], SHELL_ALPHA );

      glTranslatef( 0.0f, 0.0f, -SHELL_SIZE );
      
      q = gluNewQuadric( );
      gluCylinder( q, 0, size + 2 * SHELL_SIZE, length, subDiv, 1 );
      gluDeleteQuadric( q );
    }
  }
}

#define RECURSE_RADIUS 0.5f
#define RECURSE_CIRCUM (2*M_PI*RECURSE_RADIUS)

/*
===============
addRecursiveEdge

Add a recursive graph edge to the scene
===============
*/
void addRecursiveEdge( vec3_t origin, vec3_t dir, float nScale,
                       float scale, float cScale, float aScale )
{
  GLUquadricObj *q;
  vec3_t        forward, rotAxis;
  float         size, nSize;
  float         alpha = scaleToAlpha( aScale );
  vec3_t        diffuse;
  float         distance = Distance( camera.origin, origin );
  int           subDiv, shaftSubDiv, arrowSubDiv;
  vec4_t        colour;
  int           i, j;
  vec3_t        u, v, w;
  vec3_t        x, y, center;
  vec3_t        x1, y1;
  float         edgeFraction, shaftFraction, arrowFraction;
  float         sizeFraction1, sizeFraction2;
  float         fractionI1, fractionI2, fractionJ;
  vec3_t        flangeNormal;
  float         radius1, radius2;

  if( scale > 1.0f )
    scale = 1.0f;

  size = edgeScaleToSize( scale );
  nSize = nodeScaleToSize( nScale );
  
  subDiv = (int)( ( ( LOD_DIST - distance ) / LOD_DIST ) *
                  (float)EDGE_SUBDIV );

  if( subDiv < 3 )
    subDiv = 3;

  subDiv = (int)( (float)subDiv * ( 1.0f + ( size / MAX_NODE_SIZE ) ) );
  shaftSubDiv = subDiv << 1;
  arrowSubDiv = subDiv >> 1;
  
  lookupColour( cScale, colour );
  glColor4f( colour[ 0 ], colour[ 1 ], colour[ 2 ], alpha );
  VectorCopy( colour, diffuse );
  diffuse[ 3 ] = 1.0f;
  
  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

  //FIXME: this should really make more sense
  VectorCopy( dir, u );
  VectorInverse( u );
  
  VectorSet( w, 1.0f, 0.0f, 0.0f );
  if( VectorCompare( dir, w ) || VectorCompare( u, w ) )
    VectorSet( w, 0.0f, 0.0f, -1.0f );
  
  CrossProduct( u, w, v );
  VectorNormalise( v );
  CrossProduct( u, v, w );
  //
  
  VectorMA( origin, RECURSE_RADIUS, dir, center );
  
  glTranslatef( center[ 0 ],
                center[ 1 ],
                center[ 2 ] );
  
  edgeFraction = ( RECURSE_CIRCUM - nSize ) / RECURSE_CIRCUM;
  
  shaftFraction = ( 1.0f - ARROW_LENGTH );
  
  //shaft
  for( i = 0; i < shaftSubDiv; i++ )
  {
    sizeFraction1 = (float)i / (float)shaftSubDiv;
    sizeFraction2 = (float)( i + 1 ) / (float)shaftSubDiv;
    fractionI1 =    shaftFraction * sizeFraction1 * edgeFraction;
    fractionI2 =    shaftFraction * sizeFraction2 * edgeFraction;

    VectorScale( v, sin( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, x );
    VectorMA( x, cos( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, u, x );
    
    VectorScale( v, sin( fractionI2 * 2 * M_PI ) * RECURSE_RADIUS, y );
    VectorMA( y, cos( fractionI2 * 2 * M_PI ) * RECURSE_RADIUS, u, y );
    
    VectorCopy( x, x1 ); VectorNormalise( x1 );
    VectorCopy( y, y1 ); VectorNormalise( y1 );
    
    glBegin( GL_TRIANGLE_STRIP );
    for( j = 0; j <= subDiv; j++ )
    {
      vec3_t off1, off2;
      
      fractionJ = (float)j / (float)subDiv;
      radius1 = sizeFraction1 * size;
      radius2 = sizeFraction2 * size;

      VectorScale( w, sin( fractionJ * 2 * M_PI ) * radius1, off1 );
      VectorMA( off1, cos( fractionJ * 2 * M_PI ) * radius1, x1, off1 );
      
      VectorScale( w, sin( fractionJ * 2 * M_PI ) * radius2, off2 );
      VectorMA( off2, cos( fractionJ * 2 * M_PI ) * radius2, y1, off2 );
      
      glNormal3f( off1[ 0 ], off1[ 1 ], off1[ 2 ] );
      glVertex3f( x[ 0 ] + off1[ 0 ],
                  x[ 1 ] + off1[ 1 ],
                  x[ 2 ] + off1[ 2 ] );
      
      glNormal3f( off2[ 0 ], off2[ 1 ], off2[ 2 ] );
      glVertex3f( y[ 0 ] + off2[ 0 ],
                  y[ 1 ] + off2[ 1 ],
                  y[ 2 ] + off2[ 2 ] );
    }
    glEnd( );
  }
  
  //flange
  fractionI1 = shaftFraction * edgeFraction;

  VectorScale( v, sin( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, x );
  VectorMA( x, cos( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, u, x );
  
  VectorCopy( x, x1 ); VectorNormalise( x1 );
  
  CrossProduct( w, x1, flangeNormal );
  glNormal3f( flangeNormal[ 0 ], flangeNormal[ 1 ], flangeNormal[ 2 ] );
  
  glBegin( GL_TRIANGLE_STRIP );
  for( j = 0; j <= subDiv; j++ )
  {
    vec3_t off1, off2;
    
    fractionJ = (float)j / (float)subDiv;

    VectorScale( w, sin( fractionJ * 2 * M_PI ) * size, off1 );
    VectorMA( off1, cos( fractionJ * 2 * M_PI ) * size, x1, off1 );
    
    VectorScale( w, sin( fractionJ * 2 * M_PI ) * size * ARROW_FLANGE, off2 );
    VectorMA( off2, cos( fractionJ * 2 * M_PI ) * size * ARROW_FLANGE, x1, off2 );
    
    glVertex3f( x[ 0 ] + off1[ 0 ],
                x[ 1 ] + off1[ 1 ],
                x[ 2 ] + off1[ 2 ] );
    
    glVertex3f( y[ 0 ] + off2[ 0 ],
                y[ 1 ] + off2[ 1 ],
                y[ 2 ] + off2[ 2 ] );
  }
  glEnd( );
  
  arrowFraction = 1.0f - shaftFraction;
  
  //arrow
  for( i = 0; i < arrowSubDiv; i++ )
  {
    sizeFraction1 = (float)i / (float)arrowSubDiv;
    sizeFraction2 = (float)( i + 1 ) / (float)arrowSubDiv;
    fractionI1 =    ( shaftFraction + arrowFraction * sizeFraction1 ) * edgeFraction;
    fractionI2 =    ( shaftFraction + arrowFraction * sizeFraction2 ) * edgeFraction;

    VectorScale( v, sin( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, x );
    VectorMA( x, cos( fractionI1 * 2 * M_PI ) * RECURSE_RADIUS, u, x );
    
    VectorScale( v, sin( fractionI2 * 2 * M_PI ) * RECURSE_RADIUS, y );
    VectorMA( y, cos( fractionI2 * 2 * M_PI ) * RECURSE_RADIUS, u, y );
    
    VectorCopy( x, x1 ); VectorNormalise( x1 );
    VectorCopy( y, y1 ); VectorNormalise( y1 );
    
    glBegin( GL_TRIANGLE_STRIP );
    for( j = 0; j <= subDiv; j++ )
    {
      vec3_t off1, off2;
      
      fractionJ = (float)j / (float)subDiv;
      radius1 = ( 1.0f - sizeFraction1 ) * size * ARROW_FLANGE;
      radius2 = ( 1.0f - sizeFraction2 ) * size * ARROW_FLANGE;

      VectorScale( w, sin( fractionJ * 2 * M_PI ) * radius1, off1 );
      VectorMA( off1, cos( fractionJ * 2 * M_PI ) * radius1, x1, off1 );
      
      VectorScale( w, sin( fractionJ * 2 * M_PI ) * radius2, off2 );
      VectorMA( off2, cos( fractionJ * 2 * M_PI ) * radius2, y1, off2 );
      
      glNormal3f( off1[ 0 ], off1[ 1 ], off1[ 2 ] );
      glVertex3f( x[ 0 ] + off1[ 0 ],
                  x[ 1 ] + off1[ 1 ],
                  x[ 2 ] + off1[ 2 ] );
      
      glNormal3f( off2[ 0 ], off2[ 1 ], off2[ 2 ] );
      glVertex3f( y[ 0 ] + off2[ 0 ],
                  y[ 1 ] + off2[ 1 ],
                  y[ 2 ] + off2[ 2 ] );
    }
    glEnd( );
  }
}
