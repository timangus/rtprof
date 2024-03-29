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

#ifndef COMMS_H
#define COMMS_H

#include <sys/socket.h>

#define RTPROF_SKT  "RTPROF_SKT"

#define INET_PREFIX "rtprof://"
#define INET_LENGTH 9
#define FILE_PREFIX "unix://"
#define FILE_LENGTH 7

int   connectToRtprof( void );
void  disconnectFromRtprof( void );
void  disconnectFromFailedRtprof( void );
#define sendFE(s,fe) send(s,(void *)&fe,sizeof(functionEvent_t),MSG_NOSIGNAL)
  
#endif
