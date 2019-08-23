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

#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

int
MS_Push(struct MoveStack **stack, int f_x, int f_y, int t_x, int t_y,
        int moves)
{
    struct MoveStack *insert;

    insert = (struct MoveStack *) malloc(sizeof(struct MoveStack));

    if (!insert)
    {
        return 0;
    }
    insert->f_x = f_x;
    insert->f_y = f_y;
    insert->t_x = t_x;
    insert->t_y = t_y;
    insert->moves = moves;
    insert->c_x = -1;
    insert->c_y = -1;
    insert->next = *stack;
    *stack = insert;
    return 1;
}

int
MS_Pop(struct MoveStack **stack)
{
    struct MoveStack *temp;
    if (!*stack)
        return 0;
    temp = *stack;
    *stack = temp->next;
    free(temp);
    return 1;
}

void
MS_Clear(struct MoveStack **stack)
{
    while (MS_Pop(stack)) ;
}

int
MS_Size(struct MoveStack *stack)
{
    int size = 0;
    while (stack)
    {
        stack = stack->next;
        size++;
    }
    return size;
}
