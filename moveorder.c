/*
   Copyright (C) 2003 Alejandro Dubrovsky
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
#include "moveorder.h"
#include "heap.h"
#include "search.h"

#define MAXORDERVALUE (1 << 30)

#define M MAXHISTORYSHIFT
static const int moveOrderValues[12][12] = {
	{0,0,0,0,0,0,0,2 << M, 2 << M, 2 << M, 2 << M, 2 << M},
	{0,0,0,0,0,0,0,2 << M, 10, 5, 5, 1},
	{0,0,0,0,0,0,0,10 << M, 2 << M, 20, 20, 2},
	{0,0,0,0,0,0,0,11 << M, 7 << M, 2 << M, 2 << M, 3},
	{0,0,0,0,0,0,0,11 << M, 7 << M, 2 << M, 2 << M, 3},
	{0,0,0,0,0,0,0,12 << M, 10 << M, 8 << M, 8 << M, 2 << M},
	
	{0,2 << M, 2 << M, 2 << M, 2 << M, 2 << M, 0,0,0,0,0,0},
	{0,2 << M, 10, 5, 5, 1, 0,0,0,0,0,0},
	{0,10 << M, 2 << M, 20, 20, 2, 0,0,0,0,0,0},
	{0,11 << M, 7 << M, 2 << M, 2 << M, 3, 0,0,0,0,0,0},
	{0,11 << M, 7 << M, 2 << M, 2 << M, 3, 0,0,0,0,0,0},
	{0,12 << M, 10 << M, 8 << M, 8 << M, 2 << M, 0,0,0,0,0,0}
	}	
	;

// FIXME:  generalise for all KILLER values
static const int killerOrderValues[KILLERS << 1] = {(2 << M) - 5, (2 << M) - 10, 
(2 << M) - 20, (2 << M) - 40};

#undef M


/* 
order moves like the following:
suggested move (usually from hash or pv)
killer
if taking piece move order tables
history tables
if not taking piece change in value of piece moved according to piece tables
*/
void orderMoveList(Board *board, Move *movelist, 
		int totalmoves, int *moveorder, Move *suggmove) {
	int counter, index, from, to, i = 0;
	Move killers[KILLERS << 1];
	heapChunk heap[MAXMOVESPERNODE], tempchunk;
	pieceType frompiece, topiece;

	while (i < KILLERS) {
		killers[i << 1] = getKiller(board,board->ply,i);
		if (board->ply >= 2) killers[(i << 1) +1] = getKiller(board,board->ply - 2,i);
		i ++;
	}
			
	counter = 0;

	for (index = 0; index < totalmoves; index++) {
		from = movelist[index].from;
		to = movelist[index].to;
		tempchunk.ptr = index;
		if ((suggmove->from == from) && 
		(suggmove->to == to)) {
			switch (movelist[index].piece) {
				case NOTHING:  
				case QUEEN:	tempchunk.value = MAXORDERVALUE; break;
				case ROOK: tempchunk.value = MAXORDERVALUE - 2; break;
				case KNIGHT: tempchunk.value = MAXORDERVALUE - 1; break;
				case BISHOP: tempchunk.value = MAXORDERVALUE - 3; break;		
				default: tempchunk.value = MAXORDERVALUE;
			}
		} else {
			topiece = board->squares[to];					
			for (i = 0; i < (KILLERS << 1); i++) {
				if ((killers[i].from == from) && (killers[i].to == to)) {
					tempchunk.value = killerOrderValues[i];
					break;
				}
			}
			if (i >= (KILLERS << 1))
				tempchunk.value = board->betaers[board->blackturn][from][to];			
			frompiece = board->squares[from];
			if (topiece != NOTHING) {
				tempchunk.value += moveOrderValues[frompiece][topiece];
			} else {
				tempchunk.value += board->piecevalues[frompiece][to] - 
				board->piecevalues[frompiece][from];
			}
		}
		insertIntoHeap(heap,&tempchunk,index);
	}
	for (index = totalmoves - 1; index >= 0; index--) {
		popHeap(heap,&tempchunk,index);
		moveorder[counter++] = tempchunk.ptr;
	}
}

inline void updateKiller(Board *b, Move *m) {
	Killer temp;
	int i = 0, score = 1, swapper;
	while ((i < KILLERS) && ((b->killers[b->ply][i].move.from != m->from) ||
	(b->killers[b->ply][i].move.to != m->to))) i++;
	if (i < KILLERS) {
		score = ++(b->killers[b->ply][i].score);
	} else {
		b->killers[b->ply][KILLERS - 1].score = 1;
		b->killers[b->ply][KILLERS - 1].move = *m;
		i--;  /*  = KILLERS - 1 */
	}
	swapper = i--;
	while ((i >= 0) && (b->killers[b->ply][i].score >= score)) i--;
	if (i >= 0) {
		temp = b->killers[b->ply][swapper];
		while (swapper > i) { 
			b->killers[b->ply][swapper] = b->killers[b->ply][swapper -1];
			swapper--;
		}
		b->killers[b->ply][i] = temp;
	}
}

inline void clearKillers(Board *b, int depth) {
	Killer temp = {NOMOVE,0};
	int i = 0;
	for (; i < KILLERS; i++) b->killers[depth][i] = temp;
}

inline Move getKiller(Board *b, int depth, int which) {
	return b->killers[depth][which].move;
}

/* decrease values in history table
Thanks to Tord Romstad for the idea */
inline void makeBetaersSmaller(Board *b) {
	int i,j,k;
	
	for (i = 0; i < 2; i++)
		for (j = 0; j < 128; j++)
			for (k = 0; k < 128; k++)
				b->betaers[i][j][k] >>= 1;

}
