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
#include "pieces.h"
#include "board.h"


const int diagtopleft[64] = {
	7, 8, 9, 10, 11, 12, 13, 14,
	6, 7, 8, 9, 10, 11, 12, 13,
	5, 6, 7, 8, 9, 10, 11, 12,
	4, 5, 6, 7, 8, 9, 10, 11,
	3, 4, 5, 6, 7, 8, 9, 10,
	2, 3, 4, 5, 6, 7, 8, 9,
	1, 2, 3, 4, 5, 6, 7, 8,
	0, 1, 2, 3, 4, 5, 6, 7
};

const int diagtopright[64] = {
	14, 13, 12, 11, 10, 9, 8, 7,
	13, 12, 11, 10, 9, 8, 7, 6,
	12, 11, 10, 9, 8, 7, 6, 5,
	11, 10, 9, 8, 7, 6, 5, 4,
	10, 9, 8, 7, 6, 5, 4, 3,
	9, 8, 7, 6, 5, 4, 3, 2,
	8, 7, 6, 5, 4, 3, 2, 1,
	7, 6, 5, 4, 3, 2, 1, 0
};


const Move NOMOVE = {0,0,NOTHING};

/* slow function to check if a piece can move from one square to another.  To be used
	in non-performance-critical bits only */
int pieceCanMove(Board *b, int from, int to) {
	Move movelist[40];
	int numbermoves, i = 0, found = 0;
	
	numbermoves = generateMoves[b->squares[from]](b, from, movelist, 0);
	while ((!found) && (i < numbermoves)) {
		found = (movelist[i].to == to);
		i++;	
	}
	return found;
}


static int knightGenerate(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
	pieceType ourcolor;
		
	ourcolor = b->colors[from];	
	newmoves = pieceMoves[WKNIGHT][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] != ourcolor) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	return index;
}

static int knightGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
	pieceType theircolor;
	
	if (b->colors[from] == WHITEP) {
		theircolor = BLACKP;
	} else {
		theircolor = WHITEP;
	}
	newmoves = pieceMoves[WKNIGHT][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] == theircolor) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	return index;
}


/* pretty much copied from Bruce Moreland's 0x88 explanation */
static int rookGenerate(Board *b, int from, Move *movelist, int index) {
	int current, numberdiff;
	int *eachdiff;
	int diffs[] = {1,-1,16,-16,0};
	Move temp;
	pieceType ourcolor;
		
	ourcolor = b->colors[from];
	temp.from = from;
	temp.piece = NOTHING;
	for (eachdiff = diffs; numberdiff = *eachdiff; ++eachdiff) {
		for (current = from + numberdiff; !(current & OUT); current += numberdiff) {
			if (b->colors[current] == NOTHING) {
				temp.to = current;
				movelist[index++] = temp;
			} else if (b->colors[current] == ourcolor) {
				goto rookout;
			} else {
				temp.to = current;
				movelist[index++] = temp;
				goto rookout;
			}					
		}
		
	}
	rookout:
	return index;
}

static int rookGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int current, numberdiff;
	int *eachdiff;
	int diffs[] = {1,-1,16,-16,0};
	Move temp;
	pieceType ourcolor;
		
	ourcolor = b->colors[from];
	temp.from = from;
	temp.piece = NOTHING;
	for (eachdiff = diffs; numberdiff=*eachdiff; ++eachdiff) {
		for (current = from + numberdiff; !(current & OUT); current += numberdiff) {
			if (b->colors[current] != NOTHING) {
				if (b->colors[current] == ourcolor) {
					goto rookcaptureout;
				} else {
					temp.to = current;
					movelist[index++] = temp;
					goto rookcaptureout;
				}
			}
		}		
	}
	rookcaptureout:
	return index;
}



/* pretty much copied from Bruce Moreland's 0x88 explanation */
static int bishopGenerate(Board *b, int from, Move *movelist, int index) {
	int current, numberdiff;
	int *eachdiff;
	int diffs[] = {17,-17,15,-15,0};
	Move temp;
	pieceType ourcolor;
		
	ourcolor = b->colors[from];
	temp.from = from;
	temp.piece = NOTHING;
	for (eachdiff = diffs; numberdiff=*eachdiff; ++eachdiff) {
		for (current = from + numberdiff; !(current & OUT); current += numberdiff) {
			if (b->colors[current] == NOTHING) {
				temp.to = current;
				movelist[index++] = temp;
			} else if (b->colors[current] == ourcolor) {
				goto bishopout;
			} else {
				temp.to = current;
				movelist[index++] = temp;
				goto bishopout;
			}					
		}
	}
	bishopout:

	return index;
}

/* pretty much copied from Bruce Moreland's 0x88 explanation */
static int bishopGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int current, numberdiff;
	int *eachdiff;
	int diffs[] = {17,-17,15,-15,0};
	Move temp;
	pieceType ourcolor;
		
	ourcolor = b->colors[from];
	temp.from = from;
	temp.piece = NOTHING;
	for (eachdiff = diffs; numberdiff=*eachdiff; ++eachdiff) {
		for (current = from + numberdiff; !(current & OUT); current += numberdiff) {
			if (b->colors[current] != NOTHING) {
				if (b->colors[current] == ourcolor) {
					goto bishopcaptureout;
				} else {
					temp.to = current;
					movelist[index++] = temp;
					goto bishopcaptureout;
				}
			}
		}
	}
	bishopcaptureout:
	return index;
}



static int queenGenerate(Board *b, int from, Move *movelist, int index) {
	int newindex;
	newindex = bishopGenerate(b,from,movelist,index);
	newindex = rookGenerate(b,from,movelist,newindex);
	return newindex;
}

static int queenGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int newindex;
	newindex = bishopGenerateCaptures(b,from,movelist,index);
	newindex = rookGenerateCaptures(b,from,movelist,newindex);
	return newindex;
}

static int wpawnGenerate(Board *b, int from, Move *movelist, int index) {
	int col;
	Move temp;
	temp.from = from;
	
	col = x88PosToCol(from);
	if (from < A7) {
		temp.piece = NOTHING;		
		if ((b->colors[from + 15] == BLACKP) || (b->palp[b->ply] == from + 15)) {
			if (col > 0) {
				temp.to = from + 15;
				movelist[index++] = temp;
			}			
		}
		if ((b->colors[from + 17] == BLACKP) || (b->palp[b->ply] == from + 17)) {
			if (col < 7) {
				temp.to = from + 17;
				movelist[index++] = temp;
			}
		}
		if (b->colors[from + 16] == NOTHING) {
			temp.to = from + 16;
			movelist[index++] = temp;
			if (from <= H2) {
				if (b->colors[from + 32] == NOTHING) {
					temp.to = from + 32;
					movelist[index++] = temp;
				}
			}
		}
	} else {
		if ((col > 0) && (b->colors[from + 15] == BLACKP)) {
			temp.to = from + 15;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if ((col < 7) && (b->colors[from + 17] == BLACKP)) {
			temp.to = from + 17;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if (b->colors[from + 16] == NOTHING) {
			temp.to = from + 16;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;
		}
	}
	return index;
}

static int bpawnGenerate(Board *b, int from, Move *movelist, int index) {
	int col;
	Move temp;
	temp.from = from;
	
	col = x88PosToCol(from);
	if (from > H2) {
		temp.piece = NOTHING;		
		if ((b->colors[from - 15] == WHITEP) || (b->palp[b->ply] == from - 15)) {
			if (col < 7) {
				temp.to = from - 15;
				movelist[index++] = temp;
			}			
		}
		if ((b->colors[from - 17] == WHITEP) || (b->palp[b->ply] == from - 17)) {
			if (col > 0) {
				temp.to = from - 17;
				movelist[index++] = temp;
			}
		}
		if (b->colors[from - 16] == NOTHING) {
			temp.to = from - 16;
			movelist[index++] = temp;
			if (from >= A7) {
				if (b->colors[from - 32] == NOTHING) {
					temp.to = from - 32;
					movelist[index++] = temp;
				}
			}
		}
	} else {
		if ((col > 0) && (b->colors[from - 17] == WHITEP)) {
			temp.to = from - 17;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if ((col < 7) && (b->colors[from - 15] == WHITEP)) {
			temp.to = from - 15;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if (b->colors[from - 16] == NOTHING) {
			temp.to = from - 16;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;
		}
	}
	return index;
}

static int wpawnGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int col;
	Move temp;
	temp.from = from;
	
	col = x88PosToCol(from);
	if (from < A7) {
		temp.piece = NOTHING;		
		if ((b->colors[from + 15] == BLACKP) || (b->palp[b->ply] == from + 15)) {
			if (col > 0) {
				temp.to = from + 15;
				movelist[index++] = temp;
			}			
		}
		if ((b->colors[from + 17] == BLACKP) || (b->palp[b->ply] == from + 17)) {
			if (col < 7) {
				temp.to = from + 17;
				movelist[index++] = temp;
			}
		}
	} else {
		if ((col > 0) && (b->colors[from + 15] == BLACKP)) {
			temp.to = from + 15;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if ((col < 7) && (b->colors[from + 17] == BLACKP)) {
			temp.to = from + 17;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		
		/* Not capture but since this is used for quiescent search, it's important
		enough to leave here */
		if (b->colors[from + 16] == NOTHING) {
			temp.to = from + 16;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;
		}
	}
	return index;
}

static int bpawnGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int col;
	Move temp;
	temp.from = from;
	
	col = x88PosToCol(from);
	if (from > H2) {
		temp.piece = NOTHING;		
		if ((b->colors[from - 15] == WHITEP) || (b->palp[b->ply] == from - 15)) {
			if (col < 7) {
				temp.to = from - 15;
				movelist[index++] = temp;
			}			
		}
		if ((b->colors[from - 17] == WHITEP) || (b->palp[b->ply] == from - 17)) {
			if (col > 0) {
				temp.to = from - 17;
				movelist[index++] = temp;
			}
		}
	} else {
		if ((col > 0) && (b->colors[from - 17] == WHITEP)) {
			temp.to = from - 17;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		if ((col < 7) && (b->colors[from - 15] == WHITEP)) {
			temp.to = from - 15;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;			
		}
		/* Not capture but since this is used for quiescent search, it's important
		enough to leave here */

		if (b->colors[from - 16] == NOTHING) {
			temp.to = from - 16;
			temp.piece = QUEEN;
			movelist[index++] = temp;
			temp.piece = ROOK;
			movelist[index++] = temp;
			temp.piece = KNIGHT;
			movelist[index++] = temp;
			temp.piece = BISHOP;
			movelist[index++] = temp;
		}
	}
	return index;
}

static int wkingGenerate(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
		
	newmoves = pieceMoves[WKING][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] != WHITEP) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	if (from == E1) {
		if ((b->wkcastle) && (b->squares[F1] == NOTHING) && 
			(b->squares[G1] == NOTHING)) {
			if (inCheck(b) == 0) {
				temp.to = F1;
				movePieceSimple(b, &temp);
				b->blackturn = 1 - b->blackturn;							
				if (inCheck(b) == 0) {
					temp.to = G1;
					movelist[index++] = temp;
				}
				b->blackturn = 1 - b->blackturn;		
				unMoveSimple(b);
			}
		}
		if ((b->wqcastle) && (b->squares[D1] == NOTHING) && 
			(b->squares[C1] == NOTHING) && (b->squares[B1] == NOTHING)) {
			if (inCheck(b) == 0) {
				temp.to = D1;
				movePieceSimple(b, &temp);
				b->blackturn = 1 - b->blackturn;							
				if (inCheck(b) == 0) {
					temp.to = C1;
					movelist[index++] = temp;
				}
				b->blackturn = 1 - b->blackturn;		
				unMoveSimple(b);
			}
		}
	}	
	return index;	
}

static int bkingGenerate(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
		
	newmoves = pieceMoves[BKING][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] != BLACKP) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	if (from == E8) {
		if ((b->bkcastle) && (b->squares[F8] == NOTHING) && 
			(b->squares[G8] == NOTHING)) {
			if (inCheck(b) == 0) {
				temp.to = F8;
				movePieceSimple(b, &temp);
				b->blackturn = 1 - b->blackturn;							
				if (inCheck(b) == 0) {
					temp.to = G8;
					movelist[index++] = temp;
				}
				b->blackturn = 1 - b->blackturn;		
				unMoveSimple(b);
			}
		}
		if ((b->bqcastle) && (b->squares[D8] == NOTHING) && 
			(b->squares[C8] == NOTHING) && (b->squares[B8] == NOTHING)) {
			if (inCheck(b) == 0) {
				temp.to = D8;
				movePieceSimple(b, &temp);
				b->blackturn = 1 - b->blackturn;							
				if (inCheck(b) == 0) {
					temp.to = C8;
					movelist[index++] = temp;
				}
				b->blackturn = 1 - b->blackturn;		
				unMoveSimple(b);
			}
		}
	}	
	return index;	
}

static int wkingGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
		
	newmoves = pieceMoves[WKING][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] == BLACKP) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	return index;	
}

static int bkingGenerateCaptures(Board *b, int from, Move *movelist, int index) {
	int *newmoves, *i;
	Move temp;
		
	newmoves = pieceMoves[BKING][from];
	temp.from = from;
	temp.piece = NOTHING;
	for (i = newmoves; *i >= 0; ++i) {
		if (b->colors[*i] == WHITEP) {
			temp.to = *i;
			movelist[index++] = temp;
		}
	}
	return index;	
}


static int nothingGenerate(Board *board, int from, Move *movelist, int index) {
	return index;
}

static int nothingGenerateCaptures(Board *board, int from, Move *movelist, int index) {
	return index;
}



void initialiseGenerateFunctions() {
	generateMoves[WKING] = &wkingGenerate;
	generateMoves[WQUEEN] = &queenGenerate;
	generateMoves[WROOK] = &rookGenerate;
	generateMoves[WBISHOP] = &bishopGenerate;
	generateMoves[WKNIGHT] = &knightGenerate;
	generateMoves[WPAWN] = &wpawnGenerate;
	generateMoves[BKING] = &bkingGenerate;
	generateMoves[BQUEEN] = &queenGenerate;
	generateMoves[BROOK] = &rookGenerate;
	generateMoves[BBISHOP] = &bishopGenerate;
	generateMoves[BKNIGHT] = &knightGenerate;
	generateMoves[BPAWN] = &bpawnGenerate;
	generateMoves[NOTHING] = &nothingGenerate;

	generateCaptures[WKING] = &wkingGenerateCaptures;
	generateCaptures[WQUEEN] = &queenGenerateCaptures;
	generateCaptures[WROOK] = &rookGenerateCaptures;
	generateCaptures[WBISHOP] = &bishopGenerateCaptures;
	generateCaptures[WKNIGHT] = &knightGenerateCaptures;
	generateCaptures[WPAWN] = &wpawnGenerateCaptures;
	generateCaptures[BKING] = &bkingGenerateCaptures;
	generateCaptures[BQUEEN] = &queenGenerateCaptures;
	generateCaptures[BROOK] = &rookGenerateCaptures;
	generateCaptures[BBISHOP] = &bishopGenerateCaptures;
	generateCaptures[BKNIGHT] = &knightGenerateCaptures;
	generateCaptures[BPAWN] = &bpawnGenerateCaptures;
	generateCaptures[NOTHING] = &nothingGenerateCaptures;

}


/* The gen* functions below are left almost as is because they were written before
the transformation to 0x88 */
static void genKnightMoves()
{
	int count, i, j, currcol, diff;
	for (i = 0; i < 64; i++) {
		count = 0;
		for (j = 0; j < 64; j++) {
			diff = j - i;
			currcol = posToCol(i);
			switch (currcol) {
			case 2:
			case 3:
			case 4:
			case 5:
				switch (diff) {
				case -17:
				case -15:
				case -10:
				case -6:
				case 6:
				case 10:
				case 15:
				case 17:
					knightMoves[standardTox88(i)][count++] = standardTox88(j);
					break;
				}
				break;
			case 0:
				switch (diff) {
				case -15:
				case -6:
				case 10:
				case 17:
					knightMoves[standardTox88(i)][count++] = standardTox88(j);
					break;
				}
				break;
			case 1:
				switch (diff) {
				case -17:
				case -15:
				case -6:
				case 10:
				case 15:
				case 17:
					knightMoves[standardTox88(i)][count++] = standardTox88(j);
					break;
				}
				break;
			case 6:
				switch (diff) {
				case -17:
				case -15:
				case -10:
				case 6:
				case 15:
				case 17:
					knightMoves[standardTox88(i)][count++] = standardTox88(j);
					break;
				}
				break;
			case 7:
				switch (diff) {
				case -17:
				case -10:
				case 6:
				case 15:
					knightMoves[standardTox88(i)][count++] = standardTox88(j);
					break;
				}
			}
		}
		knightMoves[standardTox88(i)][count] = -1;
	}

}

static void genQueenMoves()
{
	int count, i, j;
	for (i = 0; i < 64; i++) {
		count = 0;
		for (j = 0; j < 64; j++) {
			if (((posToCol(j) == posToCol(i))
				  || (posToRow(j) == posToRow(i))
				  || (diagtopright[j] == diagtopright[i])
				  || (diagtopleft[j] == diagtopleft[i])) && (j != i))
				queenMoves[standardTox88(i)][count++] = standardTox88(j);
		}
		queenMoves[standardTox88(i)][count] = -1;
	}
}

static void genRookMoves()
{
	int count, i, j;
	for (i = 0; i < 64; i++) {
		count = 0;
		for (j = 0; j < 64; j++) {
			if (((posToCol(j) == posToCol(i)) ||
				  (posToRow(j) == posToRow(i))) && (i != j))
				rookMoves[standardTox88(i)][count++] = standardTox88(j);
		}
		rookMoves[standardTox88(i)][count] = -1;
	}
}

static void genBishopMoves()
{
	int count, i, j;
	for (i = 0; i < 64; i++) {
		count = 0;
		for (j = 0; j < 64; j++) {
			if (((diagtopright[j] == diagtopright[i]) ||
				  (diagtopleft[j] == diagtopleft[i])) && (i != j))
				bishopMoves[standardTox88(i)][count++] = standardTox88(j);
		}
		bishopMoves[standardTox88(i)][count] = -1;
	}
}

static void genKingMoves()
{
	int wcount, bcount, i;
	for (i = 0; i < 64; i++) {
		wcount = bcount = 0;
		if (posToRow(i) > 0) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i - 8);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i - 8);
		}
		if (posToRow(i) < 7) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i + 8);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i + 8);
		}
		if (posToCol(i) > 0) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i - 1);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i - 1);
		}
		if (posToCol(i) < 7) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i + 1);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i + 1);
		}
		if ((posToRow(i) > 0) && (posToCol(i) > 0)) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i - 9);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i - 9);
		}
		if ((posToRow(i) > 0) && (posToCol(i) < 7)) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i - 7);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i - 7);
		}
		if ((posToRow(i) < 7) && (posToCol(i) > 0)) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i + 7);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i + 7);
		}
		if ((posToRow(i) < 7) && (posToCol(i) < 7)) {
			wkingMoves[standardTox88(i)][wcount++] = standardTox88(i + 9);
			bkingMoves[standardTox88(i)][bcount++] = standardTox88(i + 9);
		}
		/* get rid of this since we are going to generate these 
		if (i == E1) {
			wkingMoves[E1][wcount++] = C1;
			wkingMoves[E1][wcount++] = G1;
		} else {
			if (i == E8) {
				bkingMoves[E8][bcount++] = C8;
				bkingMoves[E8][bcount++] = G8;
			}
		} */
		wkingMoves[standardTox88(i)][wcount] = -1;
		bkingMoves[standardTox88(i)][bcount] = -1;
	}

}


static void genPawnMoves()
{
	int wcount, bcount, i;
	for (i = 0; i < 64; i++) {
		wcount = bcount = 0;
		if ((i >= 8) && (i < 56)) {

			if (posToCol(i) > 0) {
				wpawnMoves[standardTox88(i)][wcount++] = standardTox88(i + 7);
				bpawnMoves[standardTox88(i)][bcount++] = standardTox88(i - 9);
			}
			if (posToCol(i) < 7) {
				wpawnMoves[standardTox88(i)][wcount++] = standardTox88(i + 9);
				bpawnMoves[standardTox88(i)][bcount++] = standardTox88(i - 7);
			}
			wpawnMoves[standardTox88(i)][wcount++] = standardTox88(i + 8);
			if (posToRow(i) == 1)
				wpawnMoves[standardTox88(i)][wcount++] = standardTox88(i + 16);

			bpawnMoves[standardTox88(i)][bcount++] = standardTox88(i - 8);
			if (posToRow(i) == 6)
				bpawnMoves[standardTox88(i)][bcount++] = standardTox88(i - 16);

		}
		wpawnMoves[standardTox88(i)][wcount] = -1;
		bpawnMoves[standardTox88(i)][bcount] = -1;
	}
}

int *getMoves(Board * b, int from)
{
	int *result;
	pieceType which;
	which = b->squares[from];
	switch (which) {
	case WPAWN:
		result = wpawnMoves[from];
		break;
	case BPAWN:
		result = bpawnMoves[from];
		break;
	case WROOK:
	case BROOK:
		result = rookMoves[from];
		break;
	case WQUEEN:
	case BQUEEN:
		result = queenMoves[from];
		break;
	case WBISHOP:
	case BBISHOP:
		result = bishopMoves[from];
		break;
	case WKNIGHT:
	case BKNIGHT:
		result = knightMoves[from];
		break;
	case WKING:
		result = wkingMoves[from];
		break;
	case BKING:
		result = bkingMoves[from];
		break;
	default:
		result = NULL;

	}
	return result;
}

static void copyGeneratedMoves() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 9; j++)
			pieceMoves[WKING][i][j] = wkingMoves[i][j];
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 9; j++)
			pieceMoves[BKING][i][j] = bkingMoves[i][j];
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 5; j++) {
			pieceMoves[WPAWN][i][j] = wpawnMoves[i][j];
			pieceMoves[BPAWN][i][j] = bpawnMoves[i][j];
		}
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 28; j++) {
			pieceMoves[WQUEEN][i][j] = queenMoves[i][j];
			pieceMoves[BQUEEN][i][j] = queenMoves[i][j];
		}
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 15; j++) {
			pieceMoves[WROOK][i][j] = rookMoves[i][j];
			pieceMoves[BROOK][i][j] = rookMoves[i][j];
		}
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 14; j++) {
			pieceMoves[WBISHOP][i][j] = bishopMoves[i][j];
			pieceMoves[BBISHOP][i][j] = bishopMoves[i][j];
		}
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 9; j++) {
			pieceMoves[WKNIGHT][i][j] = knightMoves[i][j];
			pieceMoves[BKNIGHT][i][j] = knightMoves[i][j];
		}
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 10; j++) {
			pieceMoves[NOTHING][i][j] = -1;
		}
	}
	
	
}

void initialiseMoveLists()
{
	int i,j;
	/* initialise all possible to moves to nothing at all */
	for (i = WKING; i <= NOTHING; i++) {
		for (j = 0; j < 128; j++) {
			pieceMoves[i][j][0] = -1;
		}
	}
	genKnightMoves();
	genRookMoves();
	genBishopMoves();
	genKingMoves();
	genQueenMoves();
	genPawnMoves();
	copyGeneratedMoves();
	
}
