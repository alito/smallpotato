/*

   Copyright (C) 2001 Alejandro Dubrovsky
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License

 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef HEAP_H
#define HEAP_H 1
#include "common.h"

/* Crappy interface for a simple heap for heapsorting, lots of handholding needed.  
	Should be neatened up but it is not a top priority */

struct heapChunkStruct
{
	int ptr, value, draw;
	U64 key;
};

typedef struct heapChunkStruct heapChunk;

void insertIntoHeap(heapChunk * heap, heapChunk * heapchunk, int lastplace);
/* Insert into the heap, heap is head of heap, heapchunk is bit to insert, lastplace
	is current size of heap */
void popHeap(heapChunk * heap, heapChunk * heapchunk, int lastplace);

#endif
