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
#include <SDL/SDL.h>
#include <sys/time.h>

#include "grph_common.h"
#include "grph_main.h"
#include "grph_primitive.h"
#include "grph_vector.h"
#include "grph_text.h"
#include "adt_graph.h"
#include "lib_comms.h"
#include "com_common.h"

#define VID_WIDTH   800
#define VID_HEIGHT  600

#define FOV         90.0f
#define ZNEAR       0.1f
#define ZFAR        100.0f

//global camera orientation
camera_t            camera;

static timeStamp_t  lastTS;
static int          width, height;

/*
===============
positionCamera

Position the camera in the scene
===============
*/
void positionCamera( void )
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  glRotatef( camera.angles[ PITCH ], 1.0f, 0.0f, 0.0f );
  glRotatef( camera.angles[ YAW ], 0.0f, 1.0f, 0.0f );
  
  glTranslatef( -camera.origin[ 0 ],
                -camera.origin[ 1 ],
                -camera.origin[ 2 ] );
}


#define NUM_AVG_FRAMES 20

/*
===============
addFPSCounter

Draw an FPS counter in the bottom left of the screen
===============
*/
void addFPSCounter( void )
{
  static int  previousFrameTimes[ NUM_AVG_FRAMES ];
  static int  index, previous;
  int         total, i;
  float       fps;
  
  previousFrameTimes[ index % NUM_AVG_FRAMES ] = camera.frameTime;
  index++;

  if( index > NUM_AVG_FRAMES )
  {
    total = 0;

    for( i = 0; i < NUM_AVG_FRAMES; i++ )
      total += previousFrameTimes[ i ];

    if( !total )
      total = 1;

    fps = 1000.0f * (float)NUM_AVG_FRAMES / (float)total;
  }
  
  glDisable( GL_LIGHTING );
  glDisable( GL_DEPTH_TEST );

  glMatrixMode( GL_PROJECTION );
  glPushMatrix( );
  glLoadIdentity( );
  gluOrtho2D( 0, width, 0, height );
        
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix( );
  glLoadIdentity( );
  glColor3f( 1.0f, 1.0f, 1.0f );

  glRasterPos2i( 10, 10 );
  glPrintf( "%.2f fps\n", fps );
  
  glPopMatrix( );
  glMatrixMode( GL_PROJECTION );
  glPopMatrix( );
  glMatrixMode( GL_MODELVIEW );
  
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_LIGHTING );
}


#define FADE_TIME 5000000.0f

/*
===============
renderScene

Render a scene
===============
*/
void renderScene( graph_t *g )
{
  vec4_t      colour, textColour;
  graphNode_t **nodes;
  graphEdge_t **edges;
  int         numNodes, numEdges;
  int         i, j;
  vec3_t      dir, dirToPos, dirToPos2;
  float       edgeLength;
  vec4_t      lightPos;
  float       aScale;
  
  nodes = listNodes( SF_NONE, &numNodes, g );
  edges = listEdges( &numEdges, g );
  
  FDPLayout( nodes, edges, numNodes, numEdges );

  glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

  addFPSCounter( );

  //light attached to the camera
  positionCamera( );
  VectorCopy( camera.origin, lightPos );
  lightPos[ 3 ] = 1.0f;
  glLightfv( GL_LIGHT1, GL_POSITION, lightPos );
  
  VectorSet( colour, 0.0f, 0.0f, 0.0f );
  
  //draw the nodes
  for( i = 0; i < numNodes; i++ )
  {
    VectorSet( textColour, 1.0f, 1.0f, 1.0f );
    
    if( !nodes[ i ]->recursiveDummy )
    {
      VectorSubtract( camera.origin, nodes[ i ]->layoutPosition, dirToPos );
      VectorNormalise( dirToPos );

      if( DotProduct( dirToPos, camera.axis[ 0 ] ) > 0.0f )
      {
        if( !nodes[ i ]->active )
          aScale = 1.0f - ( (float)nodes[ i ]->inactiveTime / FADE_TIME );
        else
          aScale = 1.0f;

        if( aScale < 0.0f )
          aScale = 0.0f;
        
        positionCamera( );
        addNode( nodes[ i ]->layoutPosition,
                 nodes[ i ]->localTimeFraction,
                 //nodes[ i ]->callsFraction,
                 aScale,
                 1.0f,
                 nodes[ i ]->textSymbol, textColour,
                 nodes[ i ]->active ? true : false, colour
               );
      }
    }
  }
  
  //draw the edges
  for( i = 0; i < numEdges; i++ )
  {
    //recursive edge
    if( edges[ i ]->to == edges[ i ]->from )
    {
      VectorSubtract( camera.origin, edges[ i ]->from->layoutPosition,
                      dirToPos );
      VectorNormalise( dirToPos );

      if( DotProduct( dirToPos, camera.axis[ 0 ] ) > 0.0f )
      {
        VectorSubtract( edges[ i ]->recursiveDummy->layoutPosition,
                        edges[ i ]->from->layoutPosition,
                        dir );
        VectorNormalise( dir );

        if( !edges[ i ]->active )
          aScale = 1.0f - ( (float)edges[ i ]->inactiveTime / FADE_TIME );
        else
          aScale = 1.0f;

        if( aScale < 0.0f )
          aScale = 0.0f;
        
        positionCamera( );
        addRecursiveEdge( edges[ i ]->from->layoutPosition, dir,
                          edges[ i ]->from->localTimeFraction,
                          0.1f,
                          /*edges[ i ]->callsFraction,*/
                          aScale,
                          1.0f
                          /*edges[ i ]->active ? 1.0f : 0.0f, colour*/ //FIXME
                        );
      }
    }
    else //normal edge
    {
      VectorSubtract( edges[ i ]->to->layoutPosition,
                      edges[ i ]->from->layoutPosition,
                      dir );

      edgeLength = VectorNormalise( dir ) -
                   nodeScaleToSize( edges[ i ]->to->localTimeFraction );

      VectorSubtract( camera.origin, edges[ i ]->from->layoutPosition,
                      dirToPos );
      VectorSubtract( camera.origin, edges[ i ]->to->layoutPosition,
                      dirToPos2 );
      VectorNormalise( dirToPos );
      VectorNormalise( dirToPos2 );
      
      if( DotProduct( dirToPos, camera.axis[ 0 ] ) > 0.0f ||
          DotProduct( dirToPos2, camera.axis[ 0 ] ) > 0.0f )
      {
        if( !edges[ i ]->active )
          aScale = 1.0f - ( (float)edges[ i ]->inactiveTime / FADE_TIME );
        else
          aScale = 1.0f;

        if( aScale < 0.0f )
          aScale = 0.0f;
        
        positionCamera( );
        addEdge( edges[ i ]->from->layoutPosition, dir, edgeLength,
                 0.1f,
                 /*edges[ i ]->callsFraction,*/
                 aScale,
                 1.0f,
                 edges[ i ]->active ? true : false, colour
               );
      }
    }
  }
  
  free( edges );
  free( nodes );
  
  SDL_GL_SwapBuffers( );
}


#define TRANSLATION_SPEED 8.0f
#define ROTATION_SPEED    0.2f
#define MAX_MOTION_HACK   50

/*
===============
GLfrontend

Input handling and rendering for the GL frontend
===============
*/
boolean GLfrontend( graph_t *g )
{
  boolean         quit = false;
  SDL_Event       ev;
  static boolean  rotating = false;
  Uint8           *keys;
  timeStamp_t     currentTS;
  float           translationUnit;

  renderScene( g );

  while( SDL_PollEvent( &ev ) )
  {
    switch( ev.type )
    {
      case SDL_QUIT:
        quit = true;
        break;
        
      case SDL_KEYDOWN:
        switch( ev.key.keysym.sym )
        {
          case SDLK_ESCAPE:
            quit = true;
            break;

          default:
            break;
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        if( ( rotating = ( ev.button.button == 1 ) ) )
        {
          SDL_WM_GrabInput( SDL_GRAB_ON );
          SDL_ShowCursor( SDL_DISABLE );
        }
        break;

      case SDL_MOUSEBUTTONUP:
        if( !( rotating = !( ev.button.button == 1 ) ) )
        {
          SDL_WM_GrabInput( SDL_GRAB_OFF );
          SDL_ShowCursor( SDL_ENABLE );
        }
        break;

      case SDL_MOUSEMOTION:
        //sometimes an erroneous event is generated right after the mouse button is
        //pressed - i suspect this is a bug in SDL. This hack filters out events
        //with large relative motion
        if( abs( ev.motion.xrel ) > MAX_MOTION_HACK ||
            abs( ev.motion.yrel ) > MAX_MOTION_HACK )
          continue;

        if( rotating )
        {
          camera.angles[ PITCH ] -= ev.motion.yrel * ROTATION_SPEED;
          camera.angles[ YAW ]   += ev.motion.xrel * ROTATION_SPEED;

          if( camera.angles[ PITCH ] >= 90.0f )
            camera.angles[ PITCH ] = 89.9f;
          else if( camera.angles[ PITCH ] <= -90.0f )
            camera.angles[ PITCH ] = -89.9f;

          AngleVectors( camera.angles, camera.axis[ 0 ],
                                       camera.axis[ 1 ],
                                       camera.axis[ 2 ] );
        }
        break;

      case SDL_VIDEORESIZE:
        if( SDL_SetVideoMode( ev.resize.w, ev.resize.h, 0, SDL_OPENGL|SDL_RESIZABLE ) == NULL )
        {
          SDL_Quit( );
          continue;
        }
        
        glViewport( 0, 0, ev.resize.w, ev.resize.h );
        
        width = ev.resize.w;
        height = ev.resize.h;

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity( );
        
        //setup the view frustrum
        gluPerspective( FOV, (GLfloat)ev.resize.w / (GLfloat)ev.resize.h, ZNEAR, ZFAR );
        
        //use modelview stack
        glMatrixMode( GL_MODELVIEW );
        break;

      default:
        break;
    }
  }

  keys = SDL_GetKeyState( NULL );

  translationUnit = ( (float)camera.frameTime / 1000.0f ) * TRANSLATION_SPEED;

  //forward/back
  if( keys[ SDLK_w ] == SDL_PRESSED )
    VectorMA( camera.origin, -translationUnit, camera.axis[ 0 ], camera.origin );
  else if( keys[ SDLK_s ] == SDL_PRESSED )
    VectorMA( camera.origin, translationUnit, camera.axis[ 0 ], camera.origin );
  
  //left/right
  if( keys[ SDLK_a ] == SDL_PRESSED )
    VectorMA( camera.origin, -translationUnit, camera.axis[ 1 ], camera.origin );
  else if( keys[ SDLK_d ] == SDL_PRESSED )
    VectorMA( camera.origin, translationUnit, camera.axis[ 1 ], camera.origin );
  
  //up/down
  if( keys[ SDLK_LSHIFT ] == SDL_PRESSED )
    camera.origin[ 1 ] += translationUnit;
  else if( keys[ SDLK_LCTRL ] == SDL_PRESSED )
    camera.origin[ 1 ] -= translationUnit;
  
  currentTS = getusecs( );
  camera.frameTime = (int)( (float)( currentTS - lastTS ) / 1000.0f );
  lastTS = currentTS;

  return quit;
}

/*
===============
initGL

Initialise the GL view
===============
*/
static GLvoid initGL( int argc, char **argv, GLsizei w, GLsizei h )
{
  GLfloat ambientLight[ 4 ] =  { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat diffuseLight[ 4 ] =  { 0.9f, 0.9f, 0.9f, 1.0f };
  GLfloat lightPosition[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat specularLight[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
  
  width = w;
  height = h;

  glViewport( 0, 0, width, height );

  //black background
  glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
  
  //enable depth buffer clearing
  glClearDepth( 1.0f );
  
  //depth test
  glDepthFunc( GL_LEQUAL );

  //enable depth test
  glEnable( GL_DEPTH_TEST );
  
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
  
  //smooth shading
  glShadeModel( GL_SMOOTH );
  
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  
  //setup the view frustrum
  gluPerspective( FOV, (GLfloat)width / (GLfloat)height, ZNEAR, ZFAR );
  
  //use modelview stack
  glMatrixMode( GL_MODELVIEW );

  //add a GL light to the scene
  glLightfv( GL_LIGHT1, GL_AMBIENT,  ambientLight );
  glLightfv( GL_LIGHT1, GL_DIFFUSE,  diffuseLight );
  glLightfv( GL_LIGHT1, GL_SPECULAR, specularLight );
  
  //light on
  glEnable( GL_LIGHT1 );
  glEnable( GL_LIGHTING );
  glEnable( GL_COLOR_MATERIAL );
  
  //normalise normals
  glEnable( GL_NORMALIZE );

  //enable back face culling
  glEnable( GL_CULL_FACE );

  //enable alpha blending
  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glutInit( &argc, argv );
}


#define CTABLE_COLOURS 2

/*
===============
initSDLandGL

Initialise SDL and a GL context
===============
*/
GLInitError_t initSDLandGL( int argc, char **argv )
{
  vec4_t colours[ CTABLE_COLOURS ];

  //initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    return GLE_SDLINIT;

  //set a GL mode
  if( SDL_SetVideoMode( VID_WIDTH, VID_HEIGHT, 0,
                        SDL_OPENGL|SDL_RESIZABLE ) == NULL )
  {
    SDL_Quit( );
    return GLE_OGLCTXT;
  }

  SDL_WM_SetCaption( "rtprof", NULL );

  initGL( argc, argv, VID_WIDTH, VID_HEIGHT );

  //initialise font
/*  if( initFont( "-adobe-helvetica-medium-r-normal--12-*-*-*-p-*-iso8859-1" ) != GLF_NONE )
    return GLE_FONT;*/

  //set up the camera
  VectorSet( camera.origin, 0.0f, 0.0f, 10.0f );
  AngleVectors( camera.angles, camera.axis[ 0 ],
                               camera.axis[ 1 ],
                               camera.axis[ 2 ] );
  
  //set the time the renderer started
  /*initialTS = getusecs( );*/

  //build the colour table
/*  VectorSet4( colours[ 0 ], 0.2f, 0.2f, 0.2f, 1.0f );
  VectorSet4( colours[ 1 ], 0.5f, 0.0f, 0.5f, 1.0f );
  VectorSet4( colours[ 2 ], 1.0f, 0.0f, 0.0f, 1.0f );
  VectorSet4( colours[ 3 ], 1.0f, 1.0f, 0.0f, 1.0f );
  VectorSet4( colours[ 4 ], 1.0f, 1.0f, 1.0f, 1.0f );
  
  buildLerpCTable( colours, CTABLE_COLOURS );*/
  
  VectorSet4( colours[ 0 ], 0.0f, 0.0f, 1.0f, 1.0f );
  VectorSet4( colours[ 1 ], 1.0f, 0.0f, 0.0f, 1.0f );
  
  buildLerpCTable( colours, CTABLE_COLOURS );

  return GLE_NONE;
}

/*
===============
shutdownSDL

Shutdown SDL and GL
===============
*/
void shutdownSDLandGL( void )
{
  /*shutdownFont( );*/
  
  SDL_Quit( );
}
