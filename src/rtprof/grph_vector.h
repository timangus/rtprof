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

#ifndef GRPH_VECTOR_H
#define GRPH_VECTOR_H

#include <math.h>

//vectors
typedef float vec_t;
typedef vec_t vec2_t[ 2 ];
typedef vec_t vec3_t[ 3 ];
typedef vec_t vec4_t[ 4 ];

#define PITCH 0
#define YAW   1
#define ROLL  2

#define DotProduct(x,y)       ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c) ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)      ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)       ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define Vector4Copy(a,b)      ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2],(b)[3]=(a)[3])
#define VectorScale(v,s,o)    ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define VectorMA(v,s,b,o)     ((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorClear(a)        ((a)[0]=(a)[1]=(a)[2]=0)
#define VectorNegate(a,b)     ((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])
#define VectorSet(v,x,y,z)    ((v)[0]=(x),(v)[1]=(y),(v)[2]=(z))
#define VectorSet4(v,w,x,y,z) ((v)[0]=(w),(v)[1]=(x),(v)[2]=(y),(v)[3]=(z))
#define DEG2RAD(a)            (((a)*M_PI)/180.0f)
#define RAD2DEG(a)            (((a)*180)/M_PI)

int   VectorCompare( const vec3_t v1, const vec3_t v2 );
vec_t VectorLength( const vec3_t v );
vec_t Distance( const vec3_t p1, const vec3_t p2 );
void  VectorInverse( vec3_t v );
void  CrossProduct( const vec3_t v1, const vec3_t v2, vec3_t cross );
vec_t VectorNormalise( vec3_t v );
void  AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up );
void  RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );

#endif
