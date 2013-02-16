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

/*
 * Sourced from the Quake series of game engines by iD software
 */

#include <math.h>
#include "grph_vector.h"

/*
===============
VectorCompare

Compare two vectors
===============
*/
int VectorCompare( const vec3_t v1, const vec3_t v2 )
{
	if( v1[ 0 ] != v2[ 0 ] ||
      v1[ 1 ] != v2[ 1 ] ||
      v1[ 2 ] != v2[ 2 ] )
  {
		return 0;
	}
  
	return 1;
}

/*
===============
VectorLength

Calculate length of a vector
===============
*/
vec_t VectorLength( const vec3_t v )
{
	return (vec_t)sqrt( v[ 0 ] * v[ 0 ] + v[ 1 ] * v[ 1 ] + v[ 2 ] * v[ 2 ] );
}

/*
===============
Distance

Distance between two points
===============
*/
vec_t Distance( const vec3_t p1, const vec3_t p2 )
{
	vec3_t	v;

	VectorSubtract( p2, p1, v );
	return VectorLength( v );
}

/*
===============
VectorInverse

Invert vector
===============
*/
void VectorInverse( vec3_t v )
{
	v[ 0 ] = -v[ 0 ];
	v[ 1 ] = -v[ 1 ];
	v[ 2 ] = -v[ 2 ];
}

/*
===============
CrossProduct

Calculate cross product of two vectors
===============
*/
void CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross )
{
	cross[ 0 ] = v1[ 1 ] * v2[ 2 ] - v1[ 2 ] * v2[ 1 ];
	cross[ 1 ] = v1[ 2 ] * v2[ 0 ] - v1[ 0 ] * v2[ 2 ];
	cross[ 2 ] = v1[ 0 ] * v2[ 1 ] - v1[ 1 ] * v2[ 0 ];
}

/*
===============
VectorNormalise

Normalise a vector
===============
*/
vec_t VectorNormalise( vec3_t v )
{
  float length, ilength;

  length = v[ 0 ] * v[ 0 ] + v[ 1 ] * v[ 1 ] + v[ 2 ] * v[ 2 ];
  length = sqrt( length );

  if( length )
  {
    ilength = 1.0f / length;
    v[ 0 ] *= ilength;
    v[ 1 ] *= ilength;
    v[ 2 ] *= ilength;
  }

  return length;
}

/*
===============
AngleVectors

Convert a set of angles to a axes
===============
*/
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up )
{
  float  angle;
  float  sr, sp, sy, cr, cp, cy;

  angle = DEG2RAD( angles[ YAW ] );
  sy = sin( angle );
  cy = cos( angle );
  angle = DEG2RAD( angles[ PITCH ] );
  sp = sin( angle );
  cp = cos( angle );
  angle = DEG2RAD( angles[ ROLL ] );
  sr = sin( angle );
  cr = cos( angle );
  
  if( forward )
  {
    forward[ 0 ] = -cp * sy;
    forward[ 1 ] = sp;
    forward[ 2 ] = cp * cy;
  }
  
  if( right )
  {
    right[ 0 ] = sr * sp * sy + cr * cy;
    right[ 1 ] = -sr * cp;
    right[ 2 ] = -sr * sp * cy + cr * sy;
  }
  
  if( up )
  {
    up[ 0 ] = -cr * sp * sy - sr * cy;
    up[ 1 ] = cr * cp;
    up[ 2 ] = -cr * sp * cy + sr * sy;
  }
}

/*
===============
RotatePointAroundVector

Rotate a point around a vector
===============
*/
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees )
{
  float sin_a;
  float cos_a;
  float cos_ia;
  float i_i_ia;
  float j_j_ia;
  float k_k_ia;
  float i_j_ia;
  float i_k_ia;
  float j_k_ia;
  float a_sin;
  float b_sin;
  float c_sin;
  float rot[ 3 ][ 3 ];

  cos_ia = DEG2RAD( degrees );
  sin_a = sin( cos_ia );
  cos_a = cos( cos_ia );
  cos_ia = 1.0f - cos_a;

  i_i_ia = dir[ 0 ] * dir[ 0 ] * cos_ia;
  j_j_ia = dir[ 1 ] * dir[ 1 ] * cos_ia;
  k_k_ia = dir[ 2 ] * dir[ 2 ] * cos_ia;
  i_j_ia = dir[ 0 ] * dir[ 1 ] * cos_ia;
  i_k_ia = dir[ 0 ] * dir[ 2 ] * cos_ia;
  j_k_ia = dir[ 1 ] * dir[ 2 ] * cos_ia;

  a_sin = dir[ 0 ] * sin_a;
  b_sin = dir[ 1 ] * sin_a;
  c_sin = dir[ 2 ] * sin_a;

  rot[ 0 ][ 0 ] = i_i_ia + cos_a;
  rot[ 0 ][ 1 ] = i_j_ia - c_sin;
  rot[ 0 ][ 2 ] = i_k_ia + b_sin;
  rot[ 1 ][ 0 ] = i_j_ia + c_sin;
  rot[ 1 ][ 1 ] = j_j_ia + cos_a;
  rot[ 1 ][ 2 ] = j_k_ia - a_sin;
  rot[ 2 ][ 0 ] = i_k_ia - b_sin;
  rot[ 2 ][ 1 ] = j_k_ia + a_sin;
  rot[ 2 ][ 2 ] = k_k_ia + cos_a;

  dst[ 0 ] = point[ 0 ] * rot[ 0 ][ 0 ] + point[ 1 ] * rot[ 0 ][ 1 ] + point[ 2 ] * rot[ 0 ][ 2 ];
  dst[ 1 ] = point[ 0 ] * rot[ 1 ][ 0 ] + point[ 1 ] * rot[ 1 ][ 1 ] + point[ 2 ] * rot[ 1 ][ 2 ];
  dst[ 2 ] = point[ 0 ] * rot[ 2 ][ 0 ] + point[ 1 ] * rot[ 2 ][ 1 ] + point[ 2 ] * rot[ 2 ][ 2 ];
}
