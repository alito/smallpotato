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

#ifndef PIECES_H
#define PIECES_H 1
#include "piecesstruct.h"
#include "board.h"



int knightMoves[128][9];
int rookMoves[128][15];
int bishopMoves[128][14];
int queenMoves[128][28];
int wkingMoves[128][9];
int bkingMoves[128][9];
int wpawnMoves[128][5];
int bpawnMoves[128][5];

int pieceMoves[NOTHING + 1][128][28];


/*typedef int (*canMoveFunction) (Board *, int, int); 
/canMoveFunction pieceCanMove[NOTHING + 1];
*/

typedef int (*generateMoveFunction) (Board *, int, Move *, int); /* from, movelist, index */
generateMoveFunction generateMoves[NOTHING + 1];
generateMoveFunction generateCaptures[NOTHING + 1];

void initialiseMoveLists();
void initialisePieceCanMoveFunctions();
void initialiseGenerateFunctions();
int pieceCanMove(Board *b, int from, int to);
int *getMoves(Board * b, int from);

#endif
