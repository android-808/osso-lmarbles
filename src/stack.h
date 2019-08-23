/***************************************************************************
                          stack.h  -  description
                             -------------------
    begin                : Thu Apr 20 2000
    copyright            : (C) 2000 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STACK_H
#define STACK_H

struct MoveStack {
    int f_x, f_y;               // from x, y //
    int t_x, t_y;               // to x, y //
    int moves;                  // moves the move takes //
    int c_x, c_y;               // crumbled wall x, y //
    struct MoveStack *next;
};

/**
 Pushes values into a new element on top of the stack.
 @param stack Pointer to a pointer to the first element (top) of the stack.
 @param f_x X-coordinate from where marble is moved.
 @param f_y Y-coordinate from where marble is moved.
 @param t_x X-coordinate to where marble is moved.
 @param t_y Y-coordinate to where marble is moved.
 @return 1 on success, 0 on failure (malloc failed)
*/
int MS_Push(struct MoveStack **stack, int f_x, int f_y, int t_x, int t_y,
            int moves);

/**
 Pops the top element off the stack.
 @param stack Pointer to a pointer to the first element (top) of the stack.
 @return 1 on success, 0 on failure (empty stack).
*/
int MS_Pop(struct MoveStack **stack);

/**
 Clears the stack
 @param stack Pointer to a pointer to the first element (top) of the stack.
*/
void MS_Clear(struct MoveStack **stack);

/**
 Counts the amount of elements in the stack.
 @param stack Pointer to the first element (top) of the stack.
 @return The amount of elements in the stack.
*/
int MS_Size(struct MoveStack *stack);

#endif
