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

#ifndef GRAPH_H
#define GRAPH_H

#include "adt_symbol.h"
#include "grph_vector.h"
#include "com_common.h"

#define MAX_BUCKETS 1024

typedef enum
{
  SF_SYMBOLP,
  SF_SYMBOLT,
  SF_TTIME,
  SF_LTIME,
  SF_CALLS,
  SF_NONE
} sortField_t;


typedef struct graphNode_s
{
  int                 id;
  void                *symbol;
  char                textSymbol[ MAX_SYMBOL_TEXT ];
  
  timeStamp_t         totalTime;
  timeStamp_t         localTime;
  float               localTimeFraction;
  float               totalTimeFraction;

  timeStamp_t         lastActive;
  timeStamp_t         inactiveTime;
  boolean             active;
  
  long                calls;
  float               callsFraction;
  
  //FIXME: these should maybe be a struct somewhere in grph_?
  vec3_t              layoutPosition;
  vec3_t              move;

  boolean             recursiveDummy;
  
  //needed for hashtable chains
  struct graphNode_s  *next;
} graphNode_t;


typedef struct graphEdge_s
{
  graphNode_t *from, *to, *recursiveDummy;
  
  timeStamp_t         lastActive;
  timeStamp_t         inactiveTime;
  boolean             active;
  
  long                calls;
  float               callsFraction;
  
  //needed for hashtable chains
  struct graphEdge_s  *next;
} graphEdge_t;


typedef struct graph_s
{
  int          numNodes;
  graphNode_t  *nodeBuckets[ MAX_BUCKETS ];

  int          numEdges;
  graphEdge_t  *edgeBuckets[ MAX_BUCKETS ];

  timeStamp_t  totalLocalTime;
  timeStamp_t  totalTotalTime;

  timeStamp_t  maxLocalTime;
  timeStamp_t  maxTotalTime;
  timeStamp_t  maxInactiveTime;

  long         maxEdgeCalls;
  long         maxNodeCalls;

  long         totalCalls;
} graph_t;


graphNode_t *searchNodes( void *symbol, void *parentSymbol, graph_t *g );
graphEdge_t *searchEdges( graphNode_t *fNode, graphNode_t *tNode, graph_t *g );

graphNode_t **listNodes( sortField_t sf, int *n, graph_t *g );
graphEdge_t **listEdges( int *n, graph_t *g );

void        initGraph( graph_t *g );
void        shutdownGraph( graph_t *g );
  
#endif
