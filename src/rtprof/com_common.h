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

#ifndef COM_COMMON_H
#define COM_COMMON_H

#define printSpaces(x) {int y,z=x;for(y=0;y<z;y++,printf(" "));}

typedef enum
{
  false,
  true
} boolean;

typedef unsigned long long timeStamp_t;

typedef enum
{
  EV_ENTER,
  EV_EXIT,
  EV_PROCEXIT
} event_t;

typedef struct functionEvent_s
{
  unsigned char type;
  void          *this_fn;
  timeStamp_t   ts;
}
#ifdef __GNUC__
__attribute__ ((packed)) //saves 3 bytes/event when using gcc
#endif
functionEvent_t;

typedef enum
{
  DL_ZERO,
  DL_ONE,
  DL_TWO,
  DL_THREE,
  DL_FOUR
} debugLevel_t;

#endif
