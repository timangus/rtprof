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

#ifndef LIB_COMMS_H
#define LIB_COMMS_H

#include "adt_graph.h"
#include "adt_stack.h"

#define MAX_HOST_NAME 64

#define RTPROF_PORT 4004
#define RTPROF_FILE "rtprof.sock"

int         acceptConnection( int type, char *socketFile );
boolean     serviceConnection( int connection, callStack_t *s, graph_t *g, int maxEvents );
timeStamp_t getusecs( void );

#endif
