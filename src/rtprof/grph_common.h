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

#ifndef GRPH_COMMON_H
#define GRPH_COMMON_H

#include "grph_vector.h"

typedef struct camera_s
{
  vec3_t origin;
  vec3_t angles;

  vec3_t axis[ 3 ];

  int    frameTime;
} camera_t;

extern camera_t camera;

void positionCamera( void );
  
#define printVector(s,v) printf("%s: %f %f %f\n",s,v[0],v[1],v[2]);

//hack to support older versions of SDL
#ifndef SDL_DISABLE
#define SDL_DISABLE 0
#endif

#ifndef SDL_ENABLE
#define SDL_ENABLE 1
#endif

#endif
