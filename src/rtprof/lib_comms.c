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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>
#include <assert.h>

#include "com_common.h"
#include "adt_graph.h"
#include "adt_stack.h"
#include "term_output.h"
#include "lib_comms.h"

//server socket
static int serverSocket;

/*
===============
getusecs

Return real time in microseconds
===============
*/
timeStamp_t getusecs( void )
{
  struct timeval tv;
    
  gettimeofday( &tv, NULL );
  
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

/*
===============
listenOnFile

Open a socket on some file
===============
*/
static int listenOnFile( char *file )
{
  int                 s;
  struct sockaddr_un  sa;
  
  unlink( file );
  memset( &sa, 0, sizeof( struct sockaddr_un ) );
  
  sa.sun_family = AF_UNIX;
  strcpy( sa.sun_path, file );
  
  if( ( s = socket( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 )
  {
    fprintf( stderr, "s < 0\n" );
    return -1;
  }
  
  if( bind( s, (struct sockaddr *)&sa, sizeof( struct sockaddr_un ) ) < 0 )
  {
    fprintf( stderr, "bind < 0 errno: %s\n", strerror( errno ) );
    close( s );
    return -1;
  }
  
  listen( s, 1 );
  return s;
}


/*
===============
listenOnPort

Open a socket on some port
===============
*/
static int listenOnPort( int port )
{
  char                myname[ MAX_HOST_NAME + 1 ];
  int                 s;
  struct sockaddr_in  sa;
  struct hostent      *hp;
  int                 so_reuseaddr = 1;
  
  memset( &sa, 0, sizeof( struct sockaddr_in ) );
  gethostname( myname, MAX_HOST_NAME );
  hp = gethostbyname( myname );
  
  if( hp == NULL )
  {
    fprintf( stderr, "hp == NULL\n" );
    return -1;
  }
  
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons( port );
  sa.sin_addr.s_addr = INADDR_ANY;
  
  if( ( s = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    fprintf( stderr, "s < 0\n" );
    return -1;
  }
  
  //set SO_REUSEADDR to prevent bind( ) problems
  if( setsockopt( s, SOL_SOCKET, SO_REUSEADDR,
                  (char *)&so_reuseaddr, sizeof( int ) ) < 0 )
  {
    fprintf( stderr, "setsockopt < 0 errno: %s\n", strerror( errno ) );
    close( s );
    return -1;
  }
  
  if( bind( s, (struct sockaddr *)&sa, sizeof( struct sockaddr_in ) ) < 0 )
  {
    fprintf( stderr, "bind < 0 errno: %s\n", strerror( errno ) );
    close( s );
    return -1;
  }
  
  listen( s, 1 );
  return s;
}

/*
===============
acceptConnection

Accept a connection from a client
===============
*/
int acceptConnection( int type, char *socketFile )
{
  int connection;
  int flags;
  
  if( type == AF_INET )
  {
    if( ( serverSocket = listenOnPort( RTPROF_PORT ) ) < 0 )
      return -1;
  }
  else if( type == AF_UNIX && socketFile )
  {
    if( ( serverSocket = listenOnFile( socketFile ) ) < 0 )
     return -1;
  }
  else
    return -1;

  if( ( connection = accept( serverSocket, 0, 0 ) ) < 0 )
    return -1;

  return connection;
}


#define SFE sizeof(functionEvent_t)

/*
===============
readFromConnection

Make a nonblocking read from connection to fe
===============
*/
static int readFromConnection( int connection, int size, functionEvent_t *fe )
{
  int count;
  
  count = recv( connection, ( (unsigned char *)fe ) + size, SFE - size, MSG_DONTWAIT );

  if( count > 0 )
    size += count;
  
  return size;
}


/*
===============
serviceConnection

Function to service a librtprof connection
maxEvents is the maximum number of events to read this call
===============
*/
boolean serviceConnection( int connection, callStack_t *s,
                           graph_t *g, int maxEvents )
{
  static functionEvent_t fe;
  static int             size = 0;
  
  boolean         clientConnected = true;
  graphNode_t     *parent, *child;
  graphEdge_t     *edge;
  void            *parentSymbol;
  stackFrame_t    sf, *sfp;
  timeStamp_t     ts, delta;
  int             count = 0;
  int             eventCount = 0;

  graphNode_t     **nodes;
  graphEdge_t     **edges;
  int             numNodes, numEdges;
  int             i;
  
  //while there are events to be processed read from the socket
  while( ( size = readFromConnection( connection, size, &fe ) ) == SFE &&
         eventCount++ < maxEvents )
  {
    child = parent = NULL;
    parentSymbol = NULL;

    assert( size == SFE );
    size = 0;

    //deal with the event
    switch( fe.type )
    {
      case EV_ENTER:
          
        if( !emptyStack( s ) )
        {
          sfp = peekStack( s );
          
          if( ( parent = searchNodes( sfp->symbol, NULL, g ) ) != NULL )
          {
            delta = ( fe.ts - sfp->calleeExitTime );
            
            parent->totalTime += delta;
            if( parent->totalTime > g->maxTotalTime )
              g->maxTotalTime = parent->totalTime;
            
            g->totalTotalTime += delta;
            
            parent->localTime += delta;
            if( parent->localTime > g->maxLocalTime )
              g->maxLocalTime = parent->localTime;

            g->totalLocalTime += delta;
          }

          sfp->calleeEntryTime = fe.ts;
          
          parentSymbol = sfp->symbol;
        }
        
        sf.symbol = fe.this_fn;
        sf.calleeExitTime = fe.ts;   
        pushStack( sf, s );
       
        if( ( child = searchNodes( sf.symbol, parentSymbol, g ) ) != NULL )
        {
          g->totalCalls++;

          child->calls++;
          child->active = true;
          child->lastActive = getusecs( );

          if( child->calls > g->maxNodeCalls )
            g->maxNodeCalls = child->calls;

          if( parent != NULL )
          {
            edge = searchEdges( parent, child, g );
            edge->calls++;
            edge->active = true;
            edge->lastActive = child->lastActive;
            
            if( edge->calls > g->maxEdgeCalls )
              g->maxEdgeCalls = edge->calls;
          }
        }
        break;

      case EV_EXIT:
        
        if( !emptyStack( s ) )
        {
          sf = popStack( s );
          
          if( ( child = searchNodes( sf.symbol, NULL, g ) ) != NULL )
          {
            delta = ( fe.ts - sf.calleeExitTime );
            
            child->totalTime += delta;
            if( child->totalTime > g->maxTotalTime )
              g->maxTotalTime = child->totalTime;
            
            g->totalTotalTime += delta;
            
            child->localTime += delta;
            if( child->localTime > g->maxLocalTime )
              g->maxLocalTime = child->localTime;

            g->totalLocalTime += delta;
            
            child->active = false;
            child->lastActive = getusecs( );
          }
          
          if( !emptyStack( s ) )
          {
            sfp = peekStack( s );
            
            if( ( parent = searchNodes( sfp->symbol, NULL, g ) ) != NULL )
            {
              delta = ( fe.ts - sfp->calleeEntryTime );
            
              parent->totalTime += delta;
              if( parent->totalTime > g->maxTotalTime )
                g->maxTotalTime = parent->totalTime;
              
              g->totalTotalTime += delta;
              
              edge = searchEdges( parent, child, g );
              edge->active = false;
              edge->lastActive = getusecs( );
            }
            
            sfp->calleeExitTime = fe.ts;
          }
        }

        break;

      case EV_PROCEXIT:
        close( connection );
        close( serverSocket );
        clientConnected = false;
        break;

      default:
        break;
    }
  }

  ts = getusecs( );
  
  nodes = listNodes( SF_NONE, &numNodes, g );
  edges = listEdges( &numEdges, g );

  g->maxInactiveTime = 0;

  for( i = 0; i < numNodes; i++ )
  {
    nodes[ i ]->inactiveTime = ts - nodes[ i ]->lastActive;

    if( nodes[ i ]->inactiveTime > g->maxInactiveTime )
      g->maxInactiveTime = nodes[ i ]->inactiveTime;

    nodes[ i ]->localTimeFraction = (float)nodes[ i ]->localTime /
                                    (float)g->totalLocalTime;
    
    nodes[ i ]->totalTimeFraction = (float)nodes[ i ]->totalTime /
                                    (float)g->totalTotalTime;
    
    nodes[ i ]->callsFraction = (float)nodes[ i ]->calls /
                                (float)g->totalCalls;
  }

  for( i = 0; i < numEdges; i++ )
  {
    edges[ i ]->inactiveTime = ts - edges[ i ]->lastActive;

    if( edges[ i ]->inactiveTime > g->maxInactiveTime )
      g->maxInactiveTime = edges[ i ]->inactiveTime;
    
    edges[ i ]->callsFraction = (float)edges[ i ]->calls /
                                (float)g->totalCalls;
  }
  
  free( edges );
  free( nodes );

  return clientConnected;
}

