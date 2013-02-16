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

#ifndef GRPH_PRIMITIVE_H
#define GRPH_PRIMITIVE_H

#include "com_common.h"
#include "grph_vector.h"

void addNode( vec3_t origin, float scale, float cScale, float aScale,
              char *name, vec3_t tc, boolean h, vec3_t colour );
void addEdge( vec3_t origin, vec3_t dir, float length, float scale,
              float cScale, float aScale, boolean h, vec3_t colour );
void addRecursiveEdge( vec3_t origin, vec3_t dir, float nScale,
                       float scale, float cScale, float aScale );

#define MIN_NODE_SIZE 0.1f
#define MAX_NODE_SIZE 0.5f

#define nodeScaleToSize(x) (MIN_NODE_SIZE+((x)*(MAX_NODE_SIZE-MIN_NODE_SIZE)))

#define MIN_EDGE_SIZE 0.03f
#define MAX_EDGE_SIZE 0.2f

#define edgeScaleToSize(x) (MIN_EDGE_SIZE+((x)*(MAX_EDGE_SIZE-MIN_EDGE_SIZE)))

#define MIN_ALPHA 0.3f
#define MAX_ALPHA 1.0f

#define scaleToAlpha(x) (MIN_ALPHA+((x)*(MAX_ALPHA-MIN_ALPHA)))

#endif
