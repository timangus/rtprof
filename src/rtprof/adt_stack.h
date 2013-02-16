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

#ifndef STACK_H
#define STACK_H

#include "com_common.h"

typedef struct stackFrame_s
{
  void                *symbol;

  timeStamp_t         calleeEntryTime;
  timeStamp_t         calleeExitTime;

  struct stackFrame_s *next;
} stackFrame_t;

typedef struct stack_s
{
  int           count;
  stackFrame_t  *top;
} callStack_t;

void          initStack( callStack_t *s );
void          shutdownStack( callStack_t *s );
void          pushStack( stackFrame_t sf, callStack_t *s );
stackFrame_t  *peekStack( callStack_t *s );
stackFrame_t  popStack( callStack_t *s );
boolean       emptyStack( callStack_t *s );

#endif
