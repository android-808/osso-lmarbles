/***************************************************************************
                          dynlist.c  -  description
                             -------------------
    begin                : Sat Apr 8 2000
    copyright            : (C) 2000 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dynlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <osso-log.h>

/* 
 * initialize list */
void
DL_Ini(DLst * dlst)
{
    dlst->cntr = 0;
    dlst->hd.p = dlst->tl.n = 0;
    dlst->hd.n = &dlst->tl;
    dlst->tl.p = &dlst->hd;
    dlst->hd.d = dlst->tl.d = 0;
    dlst->flgs = DL_NONE;
    dlst->cb = 0;
}

/* 
 * insert an item at index */
int
DL_Ins(DLst * dlst, unsigned int index, void *item)
{
    unsigned int i;
    DL_E *cur = &dlst->hd;
    DL_E *new_entry;

    if (index > dlst->cntr)
    {
        ULOG_ERR("ERR: dl_insert: index %i out of range...\n", index);
        return 1;
    }
    if (item == 0)
    {
        ULOG_ERR("ERR: dl_insert: item is NULL...\n");
        return 1;
    }

    for (i = 0; i < index; i++)
        cur = cur->n;
    new_entry = (DL_E *) malloc(sizeof(DL_E));
    new_entry->d = item;
    new_entry->n = cur->n;
    new_entry->p = cur;
    cur->n->p = new_entry;
    cur->n = new_entry;
    dlst->cntr++;

    return 0;
}

/* 
 * insert at the end of the list */
int
DL_Add(DLst * dlst, void *item)
{
    return DL_Ins(dlst, dlst->cntr, item);
}

/* 
 * delete an entry */
int
DL_DelE(DLst * dlst, DL_E * e)
{
    if (e == 0)
    {
        ULOG_ERR("ERR: dl_delete: entry is NULL...\n");
        return 1;
    }
    if (dlst->cntr == 0)
    {
        ULOG_ERR("ERR: dl_delete: list is empty...\n");
        return 1;
    }
    if (e == &dlst->hd || e == &dlst->tl)
    {
        ULOG_ERR("ERR: dl_delete: trying to delete hd or tl..\n");
        return 1;
    }

    e->p->n = e->n;
    e->n->p = e->p;
    dlst->cntr--;

    if (dlst->flgs & DL_AUTODEL)
    {
        if (dlst->flgs & DL_NOCB) {
	    if(NULL != e->d) {
                free(e->d);
		e->d = NULL;
	    }
	}
        else
        {
            if (dlst->cb == 0)
            {
                ULOG_ERR
                    ("ERR: dl_delete: no destroy callback installed...\n");
		if(NULL != e) {
                    free(e);
		    e = NULL;
		}
                return 1;
            }
            (dlst->cb) (e->d);
        }
    }
    if(NULL != e) {
        free(e);
	e = NULL;
    }

    return 0;
}

/* 
 * delete entry containing this item */
int
DL_DelP(DLst * dlst, void *item)
{
    unsigned int i;
    DL_E *cur = &dlst->hd;

    if (item == 0)
    {
        ULOG_ERR("ERR: dl_delete: item is NULL...\n");
        return 1;
    }
    if (dlst->cntr == 0)
    {
        ULOG_ERR("ERR: dl_delete: list is empty...\n");
        return 1;
    }

    for (i = 0; i <= dlst->cntr; i++)
        if (cur->n != &dlst->tl)
        {
            cur = cur->n;
            if (cur->d == item)
                break;
        }
        else
        {
            ULOG_ERR("ERR: dl_delete: list does not contain item 0x%x...\n",
                     (int) item);
            return 1;
        }

    cur->n->p = cur->p;
    cur->p->n = cur->n;
    dlst->cntr--;
    cur->n = cur->p = 0;

    if (dlst->flgs & DL_AUTODEL)
    {
        if (dlst->flgs & DL_NOCB) {
	    if(NULL != cur->d) {
                free(cur->d);
		cur->d = NULL;
	    }
	}
        else
        {
            if (dlst->cb == 0)
            {
                ULOG_ERR
                    ("ERR: dl_delete: no destroy callback installed...\n");
		if(NULL != cur) {
                    free(cur);
		    cur = NULL;
		}
                return 1;
            }
            (dlst->cb) (cur->d);
        }
    }
    if(NULL != cur) {
        free(cur);
	cur = NULL;
    }

    return 0;
}

/* 
 * delete item at index */
int
DL_Del(DLst * dlst, unsigned int index)
{
    unsigned int i;
    DL_E *cur = &dlst->hd;

    if (index >= dlst->cntr)
    {
        ULOG_ERR("ERR: dl_delete: index %i out of range...\n", index);
        return 1;
    }
    if (dlst->cntr == 0)
    {
        ULOG_ERR("ERR: dl_delete: list is empty...\n");
        return 1;
    }

    for (i = 0; i <= index; i++)
        cur = cur->n;

    cur->n->p = cur->p;
    cur->p->n = cur->n;
    dlst->cntr--;
    cur->n = cur->p = 0;

    if (dlst->flgs & DL_AUTODEL)
    {
        if (dlst->flgs & DL_NOCB) {
	    if(NULL != cur->d) {
                free(cur->d);
		cur->d = NULL;
	    }
	}
        else
        {
            if (dlst->cb == 0)
            {
                ULOG_ERR
                    ("ERR: dl_delete: no destroy callback installed...\n");
		if(NULL != cur) {
                    free(cur);
		    cur = NULL;
		}
                return 1;
            }
            (dlst->cb) (cur->d);
        }
    }
    if(NULL != cur) {
        free(cur);
	cur = NULL;
    }

    return 0;
}

/* 
 * get the item with index 'index' */
void *
DL_Get(DLst * dlst, int index)
{
    unsigned int i;
    DL_E *cur = &dlst->hd;

    if ((unsigned) index >= dlst->cntr)
    {
        ULOG_ERR("ERR: dl_get: index %i out of range...\n", index);
        return 0;
    }

    for (i = 0; i <= (unsigned) index; i++)
        cur = cur->n;

    return cur->d;
}

/* 
 * get the current entry from a pointer */
DL_E *
DL_GetE(DLst * dlst, void *item)
{
    unsigned int i;
    DL_E *cur = &dlst->hd;

    if (item == 0)
    {
        ULOG_ERR("ERR: DL_GetE: item is NULL...\n");
        return 0;
    }
    if (dlst->cntr == 0)
    {
        ULOG_ERR("ERR: DL_GetE: list is empty...\n");
        return 0;
    }

    for (i = 0; i <= dlst->cntr; i++)
        if (cur->n != &dlst->tl)
        {
            cur = cur->n;
            if (cur->d == item)
                break;
        }
        else
        {
            ULOG_ERR("ERR: DL_GetE: list does not contain item 0x%x...\n",
                     (int) item);
            return 0;
        }

    return cur;
}

/* 
 * clear all entries of list */
void
DL_Clr(DLst * dlst)
{
    DL_E *cur = dlst->hd.n;
    DL_E *n;

    while (cur != &dlst->tl)
    {
        n = cur->n;
        DL_DelE(dlst, cur);
        cur = n;
    }

}
