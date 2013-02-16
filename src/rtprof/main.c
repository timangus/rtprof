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
#include <signal.h>
#include <getopt.h>
#include <sys/socket.h>

#include "com_common.h"
#include "adt_graph.h"
#include "adt_stack.h"
#include "term_output.h"
#include "lib_comms.h"
#include "grph_main.h"

static debugLevel_t dl = DL_ZERO;

static callStack_t  callStack;
static graph_t      callGraph;

#define MAX_FILENAME_LENGTH 1024

static boolean      writeDotFile = false;
static char         dotFile[ MAX_FILENAME_LENGTH ];
static boolean      disableGL = false;
static boolean      GLstarted = false;

static char         socketFile[ MAX_FILENAME_LENGTH ];
static boolean      fileSocket = false;

/*
===============
parseOptions

Parse command line options
===============
*/
static void parseOptions( int argc, char **argv )
{
  int c;

  opterr = 0;
  
  while( 1 )
  {
    int optionIndex = 0;
    
    static struct option longOptions[ ] =
    {
      { "dotfile",      2, NULL, 'd' },
      { "disable-gl",   0, NULL, 'g' },
      { "socket",       1, NULL, 's' },
      { 0, 0, 0, 0 }
    };

    if( ( c = getopt_long( argc, argv, "d::gs:",
        longOptions, &optionIndex ) ) == -1 )
      break;
      
    switch( c )
    {
      case 'd':
        writeDotFile = true;
        
        if( optarg )
          strncpy( dotFile, optarg, MAX_FILENAME_LENGTH );
        else
          strncpy( dotFile, "callgraph.dot", MAX_FILENAME_LENGTH );
        break;
      
      case 'g':
        disableGL = true;
        break;
      
      case 's':
        fileSocket = true;
        
        strncpy( socketFile, optarg, MAX_FILENAME_LENGTH );
        break;
      
      case '?':
        fprintf( stderr, "rtprof: unrecognised option -- %c\n", optopt );
        break;

      default:
        fprintf( stderr, "rtprof: something \"bad\" happened "
                         "while parsing options\n" );
    }
  }

  //everything else on the command line is a bin file
  while( optind < argc )
    resolveSymbols( argv[ optind++ ] );
}

/*
===============
cleanUp

Exit handler
===============
*/
static void cleanUp( int signal )
{
  if( writeDotFile )
    dotOutput( dotFile, &callGraph );
  
  if( !disableGL && GLstarted )
  {
    shutdownSDLandGL( );
    GLstarted = false;
  }

  shutdownSymbolTable( );
  shutdownStack( &callStack );
  shutdownGraph( &callGraph );

  exit( 0 );
}


/*
===============
startUp

Startup handler
===============
*/
static void startUp( int argc, char **argv )
{
  int i;
  
  initGraph( &callGraph );
  initStack( &callStack );
  initSymbolTable( );

  parseOptions( argc, argv );
}


/*
===============
main

Main program
===============
*/
int main( int argc, char **argv )
{
  int     connection = -1;
  boolean quit = false;
  boolean clientConnected = false;
  
  startUp( argc, argv );
  signal( SIGINT, cleanUp );
  
  fprintf( stderr, "rtprof: waiting for client connection... " );

  if( fileSocket )
    connection = acceptConnection( AF_UNIX, socketFile );
  else
    connection = acceptConnection( AF_INET, NULL );

  if( connection >= 0 )
  {
    fprintf( stderr, "accepted\n" );
    clientConnected = true;
  }
  else
    fprintf( stderr, "failed\n" );
  
  if( !disableGL )
  {
    initSDLandGL( argc, argv );
    GLstarted = true;
  }
  
  while( !quit )
  {
    if( clientConnected )
      clientConnected = serviceConnection( connection, &callStack, &callGraph, 10000 );

    if( !disableGL )
      quit = GLfrontend( &callGraph );
    else if( !clientConnected )
      quit = true;
  }
  
  cleanUp( 0 );
  
  return 0;
}
   
