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
#include <sys/time.h>

#include "comms.h"
#include "../rtprof/com_common.h"

//static to avoid stack frame juggling
static struct timeval   tv;
static functionEvent_t  fe;
int                     connection = -1;
static boolean          attemptedConnection = false;

/*
===============
__cyg_profile_func_enter

Instrumentation function called on entry
===============
*/
void __cyg_profile_func_enter( void *this_fn, void *call_site )
{
  if( connection < 0 && !attemptedConnection )
  {
    attemptedConnection = true;

    if( ( connection = connectToRtprof( ) ) < 0 )
      fprintf( stderr, "WARNING: librtprof cannot connect to rtprof\n" );
    else
      atexit( disconnectFromRtprof );
  }
  
  if( connection >= 0 )
  {
    gettimeofday( &tv, NULL );

    fe.type = EV_ENTER;
    fe.this_fn = this_fn;
    fe.ts = tv.tv_sec * 1000000 + tv.tv_usec;

    if( sendFE( connection, fe ) < 0 )
    {
      disconnectFromFailedRtprof( );
      fprintf( stderr, "WARNING: could not send to rtprof; disconnected\n" );
    }
  }
}

/*
===============
__cyg_profile_func_exit

Instrumentation function called on exit
===============
*/
void __cyg_profile_func_exit( void *this_fn, void *call_site )
{
  if( connection >= 0 )
  {
    gettimeofday( &tv, NULL );

    fe.type = EV_EXIT;
    fe.this_fn = this_fn;
    fe.ts = tv.tv_sec * 1000000 + tv.tv_usec;

    if( sendFE( connection, fe ) < 0 )
    {
      disconnectFromFailedRtprof( );
      fprintf( stderr, "WARNING: could not send to rtprof; disconnected\n" );
    }
  }
}
