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

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netdb.h>

#include "comms.h"
#include "../rtprof/com_common.h"
#include "../rtprof/lib_comms.h"

extern int connection;

/*
===============
parseSocketVariable
===============
*/
static int parseSocketVariable( char *buffer, int bufferSize )
{
  char *env;

  env = getenv( RTPROF_SKT );

  if( env != NULL )
  {
    if( !strncmp( env, INET_PREFIX, INET_LENGTH ) )
    {
      strncpy( buffer, env + INET_LENGTH, bufferSize - INET_LENGTH );
      return AF_INET;
    }
    else if( !strncmp( env, FILE_PREFIX, FILE_LENGTH ) )
    {
      strncpy( buffer, env + FILE_LENGTH, bufferSize - FILE_LENGTH );
      return AF_UNIX;
    }
    else
      return -1;
  }
  else
    return -1;
}


/*
===============
disconnectFromRtprof

Disconnect from analysis program
===============
*/
void disconnectFromRtprof( void )
{
  functionEvent_t fe;

  fe.type = EV_PROCEXIT;

  sendFE( connection, fe );
  close( connection );
}


/*
===============
disconnectFromFailedRtprof

Disconnect from failed analysis program
===============
*/
void disconnectFromFailedRtprof( void )
{
  close( connection );
  connection = -1;
}


#define MAX_HB  160

/*
===============
connectToRtprof

Connect to analysis program
===============
*/
int connectToRtprof( void )
{
  int   socketType;
  char  hostBuffer[ MAX_HB ];
  int   s;

  socketType = parseSocketVariable( hostBuffer, MAX_HB );

  if( socketType == AF_INET )
  {
    //tcp socket
    struct sockaddr_in sa;
    struct hostent *hp;
    
    if( ( hp = gethostbyname( hostBuffer ) ) == NULL )
      return -1;
    
    memset( &sa, 0, sizeof( sa ) );
    
    memcpy( (char *)&sa.sin_addr, hp->h_addr, hp->h_length );
    sa.sin_family = hp->h_addrtype;
    sa.sin_port = htons( (u_short)RTPROF_PORT );
    
    if( ( s = socket( hp->h_addrtype, SOCK_STREAM, 0 ) ) < 0 )
      return -1;
    
    if( connect( s, (struct sockaddr *)&sa, sizeof( sa ) ) < 0 )
    {
      close( s );
      return -1;
    }

    return s;
  }
  else if( socketType == AF_UNIX )
  {
    //file socket
    struct sockaddr_un sa;
    
    memset( &sa, 0, sizeof( sa ) );
    
    strcpy( sa.sun_path, hostBuffer );
    sa.sun_family = AF_UNIX;
    
    if( ( s = socket( AF_UNIX, SOCK_STREAM, 0 ) ) < 0 )
      return -1;

    if( connect( s, (struct sockaddr *)&sa, sizeof( sa ) ) < 0 )
    {
      close( s );
      return -1;
    }

    return s;
  }
  else if( socketType < 0 )
  {
    fprintf( stderr, "WARNING: librtprof cannot parse environment"
                     "variable RTPROF_SKT\n" );
    return -1;
  }

  return -1;
}
