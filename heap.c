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
#include "heap.h"

void
insertIntoHeap(heapChunk * heap, heapChunk * heapchunk, int lastplace)
{
	int position, parent;
	position = lastplace;

	parent = (position - 1) >> 1;
	while ((position > 0) && (heapchunk->value > heap[parent].value)) {
		heap[position] = heap[parent];
		position = parent;
		parent = (position - 1) >> 1;
	}

	heap[position] = *heapchunk;
}



void popHeap(heapChunk * heap, heapChunk * heapchunk, int lastplace)
{
	int bigchild, child1 = 1, child2 = 2, position = 0;
	*heapchunk = *heap;
	if (heap[1].value >= heap[2].value) {
		bigchild = 1;
	} else {
		bigchild = 2;
	}
	while ((child1 < lastplace)
			 && (heap[lastplace].value < heap[bigchild].value)) {
		heap[position] = heap[bigchild];
		position = bigchild;
		child1 = (position << 1) + 1;
		child2 = (position << 1) + 2;
		if (child2 >= lastplace) {
			bigchild = child1;
		} else {
			if (heap[child1].value >= heap[child2].value) {
				bigchild = child1;
			} else {
				bigchild = child2;
			}

		}
	}
	heap[position] = heap[lastplace];

}
