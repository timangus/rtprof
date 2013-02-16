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
#include "adt_graph.h"
#include "adt_symbol.h"

/*
===============
addToNodeBucket

Add a new graphNode_t to a bucket chain
===============
*/
static graphNode_t *addToNodeBucket( graphNode_t *bucket, void *symbol )
{
  graphNode_t *head = bucket;

  if( bucket )
  {
    //already elements in the list
    while( bucket->next )
      bucket = bucket->next;
      
    bucket->next = (graphNode_t *)malloc( sizeof( graphNode_t ) );
    bucket = bucket->next;
    
    memset( bucket, 0, sizeof( graphNode_t ) );
    bucket->symbol = symbol;
    bucket->next = NULL;
    
    return head;
  }
  else
  {
    //list is empty
    bucket = (graphNode_t *)malloc( sizeof( graphNode_t ) );
    
    memset( bucket, 0, sizeof( graphNode_t ) );
    bucket->symbol = symbol;
    bucket->next = NULL;
    
    return bucket;
  }
}

/*
===============
findNodeInChain

Return a graphNode_t based on 
===============
*/
static graphNode_t *findNodeInChain( graphNode_t *bucket, void *symbol )
{
  graphNode_t *p = bucket;

  while( p )
  {
    if( p->symbol == symbol )
      return p;
      
    p = p->next;
  }
  
  return NULL;
}

/*
===============
removeNodeFromChain

Remove a graphNode_t based on 
===============
*/
static graphNode_t *removeNodeFromChain( graphNode_t *bucket, void *symbol )
{
  graphNode_t *head = bucket;
  graphNode_t *p;
  
  while( bucket )
  {
    if( bucket->symbol == symbol )
    {
      p = head;

      while( p->next != bucket )
        p = p->next;
      
      p->next = bucket->next;
      free( bucket );
      
      return head;
    }
      
    bucket = bucket->next;
  }
  
  return NULL;
}


/*
===============
addToEdgeBucket

Add a new graphEdge_t to a bucket chain
===============
*/
static graphEdge_t *addToEdgeBucket( graphEdge_t *bucket, graphNode_t *fNode,
                                     graphNode_t *tNode )
{
  graphEdge_t *head = bucket;

  if( bucket )
  {
    //already elements in the list
    while( bucket->next )
      bucket = bucket->next;
      
    bucket->next = (graphEdge_t *)malloc( sizeof( graphEdge_t ) );
    bucket = bucket->next;
    
    memset( bucket, 0, sizeof( graphEdge_t ) );
    bucket->from = fNode;
    bucket->to = tNode;
    bucket->next = NULL;
    
    return head;
  }
  else
  {
    //list is empty
    bucket = (graphEdge_t *)malloc( sizeof( graphEdge_t ) );
    
    memset( bucket, 0, sizeof( graphEdge_t ) );
    bucket->from = fNode;
    bucket->to = tNode;
    bucket->next = NULL;
    
    return bucket;
  }
}

/*
===============
findEdgeInChain

Return a graphEdge_t based on 
===============
*/
static graphEdge_t *findEdgeInChain( graphEdge_t *bucket, graphNode_t *fNode,
                                     graphNode_t *tNode )
{
  graphEdge_t *p = bucket;

  while( p )
  {
    if( p->from == fNode && p->to == tNode )
      return p;
      
    p = p->next;
  }
  
  return NULL;
}

/*
===============
removeEdgeFromChain

Remove a graphEdge_t based on 
===============
*/
static graphEdge_t *removeEdgeFromChain( graphEdge_t *bucket,
                                         graphNode_t *fNode,
                                         graphNode_t *tNode )
{
  graphEdge_t *head = bucket;
  graphEdge_t *p;
  
  while( bucket )
  {
    if( bucket->from == fNode && bucket->to == tNode )
    {
      p = head;

      while( p->next != bucket )
        p = p->next;
      
      p->next = bucket->next;
      free( bucket );
      
      return head;
    }
      
    bucket = bucket->next;
  }
  
  return NULL;
}


#define DIST_RANGE 1.0f
#define RANDOMDIST (-(DIST_RANGE*0.5f)+(((float)random()/ \
                   (float)RAND_MAX)*DIST_RANGE))
    
/*
===============
searchNodes

Search for a graph node in the bucket-chain hash
and allocate a new one if it doesn't exist
===============
*/
graphNode_t *searchNodes( void *symbol, void *parentSymbol, graph_t *g )
{
  graphNode_t *node, *parentNode;
  int         index = (int)( (long)symbol % MAX_BUCKETS );
  char        *t;
  
  node = findNodeInChain( g->nodeBuckets[ index ], symbol );

  if( node == NULL )
  {
    g->nodeBuckets[ index ] = addToNodeBucket( g->nodeBuckets[ index ],
                                               symbol );
    
    node = findNodeInChain( g->nodeBuckets[ index ], symbol );
    node->id = g->numNodes++;

    if( ( t = lookupSymbol( symbol ) ) != NULL )
      snprintf( node->textSymbol, MAX_SYMBOL_TEXT, "%s", t );
    else
      snprintf( node->textSymbol, MAX_SYMBOL_TEXT, "%p", symbol );

    //seed the random number generator so the random
    //distribution is deterministic
    /*srandom( *(unsigned int *)symbol );*/
    VectorSet( node->layoutPosition, RANDOMDIST, RANDOMDIST, RANDOMDIST );
    
    //try to place this new node near the node that called it
    if( parentSymbol != NULL )
    {
      index = (int)( (long)parentSymbol % MAX_BUCKETS );
      parentNode = findNodeInChain( g->nodeBuckets[ index ], parentSymbol );
    
      VectorAdd( node->layoutPosition,
                 parentNode->layoutPosition,
                 node->layoutPosition );
    }
  }
    
  return node;
}


/*
===============
searchEdges

Search for a graph edge in the bucket-chain hash
and allocate a new one if it doesn't exist
===============
*/
graphEdge_t *searchEdges( graphNode_t *fNode, graphNode_t *tNode, graph_t *g )
{
  graphEdge_t *edge;
  int         index = (int)( ( (long)fNode + (long)tNode ) % MAX_BUCKETS );
  
  edge = findEdgeInChain( g->edgeBuckets[ index ], fNode, tNode );

  if( edge == NULL )
  {
    g->edgeBuckets[ index ] = addToEdgeBucket( g->edgeBuckets[ index ],
                                               fNode, tNode );
    
    g->numEdges++;

    edge = findEdgeInChain( g->edgeBuckets[ index ], fNode, tNode );

    //this is a recursive edge
    if( fNode == tNode )
    {
      graphNode_t *dummyNode;
      void        *dummySymbol = fNode + 1;
      
      //add a dummy node to aid layout of the edge
      g->nodeBuckets[ index ] = addToNodeBucket( g->nodeBuckets[ index ],
                                                 dummySymbol );

      dummyNode = findNodeInChain( g->nodeBuckets[ index ], dummySymbol );
      dummyNode->id = g->numNodes++;

      /*srandom( *(unsigned int *)dummySymbol );*/
      VectorSet( dummyNode->layoutPosition, RANDOMDIST,
                                            RANDOMDIST,
                                            RANDOMDIST );
      VectorAdd( dummyNode->layoutPosition,
                 fNode->layoutPosition,
                 dummyNode->layoutPosition );
      
      dummyNode->recursiveDummy = true;
      edge->recursiveDummy = dummyNode;
    }
  }
    
  return edge;
}


/*
===============
initGraph

Initialise some stuff for the graph
===============
*/
void initGraph( graph_t *g )
{
  int i;

  for( i = 0; i < MAX_BUCKETS; i++ )
    g->nodeBuckets[ i ] = NULL;
  
  for( i = 0; i < MAX_BUCKETS; i++ )
    g->edgeBuckets[ i ] = NULL;
}

/*
===============
shutdownGraph

Shutdown some stuff
===============
*/
void shutdownGraph( graph_t *g )
{
  int         i;
  graphNode_t *p, *q;
  graphEdge_t *r, *s;

  for( i = 0; i < MAX_BUCKETS; i++ )
  {
    p = g->nodeBuckets[ i ];
    
    while( p )
    {
      q = p->next;
      
      free( p );

      p = q;
    }
  }
  
  for( i = 0; i < MAX_BUCKETS; i++ )
  {
    r = g->edgeBuckets[ i ];
    
    while( r )
    {
      s = r->next;
      
      free( r );

      r = s;
    }
  }
}


/*
===============
cmpGraphNodeSymbol

Compare graphNode_t on total time
===============
*/
static int cmpGraphNodeSymbol( const void *v1, const void *v2 )
{
  graphNode_t *n1 = *(graphNode_t **)v1;
  graphNode_t *n2 = *(graphNode_t **)v2;
  
  if( n1->symbol < n2->symbol )
    return -1;
  else if( n1->symbol > n2->symbol )
    return 1;
  else
    return 0;
}


/*
===============
cmpGraphNodeTTime

Compare graphNode_t on total time
===============
*/
static int cmpGraphNodeTTime( const void *v1, const void *v2 )
{
  graphNode_t *n1 = *(graphNode_t **)v1;
  graphNode_t *n2 = *(graphNode_t **)v2;
  
  if( n1->totalTime < n2->totalTime )
    return -1;
  else if( n1->totalTime > n2->totalTime )
    return 1;
  else
    return 0;
}


/*
===============
cmpGraphNodeLTime

Compare graphNode_t on local time
===============
*/
static int cmpGraphNodeLTime( const void *v1, const void *v2 )
{
  graphNode_t *n1 = *(graphNode_t **)v1;
  graphNode_t *n2 = *(graphNode_t **)v2;
  
  if( n1->localTime < n2->localTime )
    return -1;
  else if( n1->localTime > n2->localTime )
    return 1;
  else
    return 0;
}


/*
===============
cmpGraphNodeCalls

Compare graphNode_t on calls
===============
*/
static int cmpGraphNodeCalls( const void *v1, const void *v2 )
{
  graphNode_t *n1 = *(graphNode_t **)v1;
  graphNode_t *n2 = *(graphNode_t **)v2;
  
  if( n1->calls < n2->calls )
    return -1;
  else if( n1->calls > n2->calls )
    return 1;
  else
    return 0;
}


/*
===============
listNodes

malloc and return a list
===============
*/
graphNode_t **listNodes( sortField_t sf, int *n, graph_t *g )
{
  int         i, j = 0;
  graphNode_t *p, **nodeArray;

  nodeArray = (graphNode_t **)malloc( g->numNodes * sizeof( graphNode_t * ) );

  for( i = 0; i < MAX_BUCKETS; i++ )
  {
    p = g->nodeBuckets[ i ];

    while( p )
    {
      nodeArray[ j++ ] = p;

      if( p->next )
        p = p->next;
      else
        p = NULL;
    }
  }

  switch( sf )
  {
    case SF_SYMBOLP:
    case SF_SYMBOLT:
      qsort( nodeArray, g->numNodes, sizeof( graphNode_t * ),
             cmpGraphNodeSymbol );
      break;
      
    case SF_TTIME:
      qsort( nodeArray, g->numNodes, sizeof( graphNode_t * ),
             cmpGraphNodeTTime );
      break;

    case SF_LTIME:
      qsort( nodeArray, g->numNodes, sizeof( graphNode_t * ),
             cmpGraphNodeLTime );
      break;

    case SF_CALLS:
      qsort( nodeArray, g->numNodes, sizeof( graphNode_t * ),
             cmpGraphNodeCalls );
      break;

    case SF_NONE:
    default:
      break;
  }

  *n = g->numNodes;
  
  return nodeArray;
}


/*
===============
listEdges

malloc and return a list
===============
*/
graphEdge_t **listEdges( int *n, graph_t *g )
{
  int         i, j = 0;
  graphEdge_t *p, **edgeArray;

  edgeArray = (graphEdge_t **)malloc( g->numEdges * sizeof( graphEdge_t * ) );

  for( i = 0; i < MAX_BUCKETS; i++ )
  {
    p = g->edgeBuckets[ i ];

    while( p )
    {
      edgeArray[ j++ ] = p;

      if( p->next )
        p = p->next;
      else
        p = NULL;
    }
  }

  *n = g->numEdges;
  
  return edgeArray;
}
