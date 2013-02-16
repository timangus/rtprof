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

#include "grph_common.h"
#include "grph_layout.h"
#include "grph_vector.h"
#include "adt_graph.h"
#include "com_common.h"

/*
===============
circleLayout

Naive layout algorithm that simply places
the nodes in a circle
===============
*/
void circleLayout( graphNode_t **nodes, int numNodes )
{
  int         i;
  float       interval;
  vec3_t      point;

  interval = ( 2.0f * M_PI ) / numNodes;

  for( i = 0; i < numNodes; i++ )
  {
    point[ 0 ] = sin( i * interval ) * numNodes * 0.1f;
    point[ 1 ] = cos( i * interval ) * numNodes * 0.1f;
    point[ 2 ] = 0.0f;

    VectorCopy( point, nodes[ i ]->layoutPosition );
  }
}


/*
===============
lnTaylorSeries

Calculate ln(x) using a taylor series
Turns out order 1 is accurate enough, so this isn't used
===============
*/
static float lnTaylorSeries( float x, int order )
{
  int   i, j, k;
  float result = 0.0f;
  float xMinus1 = x - 1.0f;
  float xPlus1  = x + 1.0f;
  float term = xMinus1 / xPlus1;
  float y;

  for( i = 0; i <= order; i++ )
  {
    k = i * 2 + 1;
    
    y = term;
    
    for( j = 1; j < k; j++ )
      y *= term;
    
    result += y * ( 1.0f / (float)k );
  }

  return 2.0f * result;
}


#define NEAR_INFINITY 99999999.0f

#define TAYLOR_LNX(x) (2.0f*(((x)-1.0f)/((x)+1.0f)))

#define FACTOR        0.6f

#define REPULSE(x)    (x)!=0.0f?(FACTOR*(1.0f/((x)*(x)))):(NEAR_INFINITY)
#define ATTRACT(x)    (FACTOR*TAYLOR_LNX(x))

/* "correct" way 
#define REPULSE(x)    (x)!=0.0f?(FACTOR*(1.0f/pow((x),2.0f))):(NEAR_INFINITY)
#define ATTRACT(x)    (FACTOR*log((x)/1.0f))
*/

#define ITERATIONS    1

/*
===============
FDPLayout

Force Directed Placement layout
Based on the algorithm outlined in P. Eades' paper (1984)
===============
*/
void FDPLayout( graphNode_t **nodes, graphEdge_t **edges, int numNodes, int numEdges )
{
  int     i, j, k;
  vec3_t  delta;
  float   distance, force;

  for( k = 0; k < ITERATIONS; k++ )
  {
    //clear the moves for all the nodes
    for( i = 0; i < numNodes; i++ )
      VectorClear( nodes[ i ]->move );
    
    //repulsive forces
    for( i = 0; i < numNodes; i++ )
    {
      for( j = 0; j < numNodes; j++ )
      {
        if( i != j )
        {
          VectorSubtract( nodes[ j ]->layoutPosition,
                          nodes[ i ]->layoutPosition,
                          delta );
          
          distance = VectorNormalise( delta );
          force = REPULSE( distance );
          
          //FIXME: if( distance == 0 ) delta is some vector
          if( distance == 0.0f )
            VectorSet( delta, 0.0f, 1.0f, 0.0f );

          VectorMA( nodes[ j ]->move, force, delta, nodes[ j ]->move );
        }
      }
    }

    //attractive forces
    for( i = 0; i < numEdges; i++ )
    {
      if( edges[ i ]->from != edges[ i ]->to )
      {
        VectorSubtract( edges[ i ]->to->layoutPosition,
                        edges[ i ]->from->layoutPosition,
                        delta );

        distance = VectorNormalise( delta );
        force = ATTRACT( distance );
        
        if( distance > 0.0f )
        {
          VectorMA( edges[ i ]->to->move, -force, delta, edges[ i ]->to->move );
          VectorMA( edges[ i ]->from->move, force, delta, edges[ i ]->from->move );
        }
      }
      else
      {
        VectorSubtract( edges[ i ]->from->layoutPosition,
                        edges[ i ]->recursiveDummy->layoutPosition,
                        delta );

        distance = VectorNormalise( delta );
        force = ATTRACT( distance );
        
        if( distance > 0.0f )
        {
          VectorMA( edges[ i ]->from->move, -force, delta, edges[ i ]->from->move );
          VectorMA( edges[ i ]->recursiveDummy->move, force, delta, edges[ i ]->recursiveDummy->move );
        }
      }
    }

    //apply the forces
    for( i = 0; i < numNodes; i++ )
    {
      VectorMA( nodes[ i ]->layoutPosition,
                 0.1f, nodes[ i ]->move,
                 nodes[ i ]->layoutPosition );
    }
  }
}
