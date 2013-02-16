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
#include "adt_stack.h"

/*
===============
initStack

Initialise a stack
===============
*/
void initStack( callStack_t *s )
{
  s->count = 0;
  s->top = NULL;
}

/*
===============
shutdownStack

Shutdown a stack
===============
*/
void shutdownStack( callStack_t *s )
{
  while( s->count >= 1 )
    popStack( s );
}

/*
===============
pushStack

Add a new stackFrame_t to the stack
===============
*/
void pushStack( stackFrame_t sf, callStack_t *s )
{
  stackFrame_t *p;

  p = (stackFrame_t *)malloc( sizeof( stackFrame_t ) );
  *p = sf;
  p->next = s->top;

  s->top = p;
  s->count++;
}

/*
===============
peekStack

Return the stackFrame_t on top of the stack
===============
*/
stackFrame_t *peekStack( callStack_t *s )
{
  return s->top;
}

/*
===============
popStack

Return the stackFrame_t on top of the stack
===============
*/
stackFrame_t popStack( callStack_t *s )
{
  stackFrame_t sf, *p;

  sf = *s->top;
  p = s->top;

  s->top = s->top->next;
  s->count--;

  free( p );

  return sf;
}

/*
===============
emptyStack

Is stack empty?
===============
*/
boolean emptyStack( callStack_t *s )
{
  return (boolean)( s->count == 0 );
}
