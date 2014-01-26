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
 

#include "common.h"
#include "hash.h"
#include "board.h"
#include <libale.h>
#include <stdlib.h>

void clearHashTable(hashTable * hashtable)
{
	unsigned size, i;
	if (hashtable != NULL) {
		size = hashtable->size;
		for (i = 0; i < size; i++) {
			hashtable->white[i].depth = -1;
			hashtable->black[i].depth = -1;
		}
	}
}

hashTable *initialiseHashTableBySize(unsigned hashkilobytes) {
	unsigned entries;
	
	entries = 1024 * hashkilobytes / (sizeof(hashChunk));
	return initialiseHashTable(entries);
}


hashTable *initialiseHashTable(unsigned hashsize)
{
	int i, j;
	hashTable *hashtable;
	if (hashsize > 0) {
		hashtable = (hashTable *) xmalloc(sizeof(hashTable));
		hashtable->size = hashsize >> 1;
		hashtable->white =
			(hashChunk *) xmalloc(sizeof(hashChunk) * hashtable->size);
		hashtable->black =
			(hashChunk *) xmalloc(sizeof(hashChunk) * hashtable->size);
		for (i = 0; i < 14; i++) {
			for (j = 0; j < 128; j++) {
				hashtable->piecekeys[i][j] = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
			}
		}
		hashtable->wqcastle = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
		hashtable->wkcastle = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
		hashtable->bqcastle = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
		hashtable->bkcastle = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
		/*hashtable->blackhash = random() % hashsize; */
		
		clearHashTable(hashtable);
	} else {
		hashtable = NULL;
	}
	return hashtable;
}

U64 getHashKey(hashTable * hashtable, Board * board)
{
	int i, wking;
	U64 key;


	key = 0L;
	wking = (int) WKING;
	i = A1;
	while (i <= H8) {
		key ^= hashtable->piecekeys[(int) board->squares[i] - wking][x88ToStandard(i)];
		i++;
		if (i & OUT) i+=8;
	}

	if (board->palp[board->ply] >= 0) {
		key ^= hashtable->piecekeys[PALPHASHKEY][board->palp[board->ply]];
	}
	
	if (!board->wkcastle) {
		key ^= hashtable->wkcastle;
	} 
	if (!board->wqcastle) {
		key ^= hashtable->wqcastle;
	}		
	if (!board->bkcastle) {
		key ^= hashtable->bkcastle;
	} 
	if (!board->bqcastle) {
		key ^= hashtable->bqcastle;
	}		

	
	/*if (board->blackturn) {
	 * key ^= hashtable->blackhash;
	 * } */
	return key;
}

