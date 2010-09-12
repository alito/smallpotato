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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef HASHSTRUCT_H
#define HASHSTRUCT_H 1

#include "piecesstruct.h"
#define PALPHASHKEY 13


struct hashChunkStruct
{
	U64 key;
	char depth;
	unsigned char flags, from, to;
	pieceType piece;	
	int lbound, ubound;
	short int ply;
};

typedef struct hashChunkStruct hashChunk;

struct hashTableStruct
{
	hashChunk *white, *black;
	unsigned size;
	U64 piecekeys[14][128];
	U64 wqcastle, wkcastle, bqcastle, bkcastle;
};

typedef struct hashTableStruct hashTable;


#endif
