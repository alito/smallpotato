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
#include "eval.h"
#include "search.h"
#include <math.h>
#include <stdlib.h>

#define ENDPHASE_TOTAL 3600  /* wild guess for now */


static const int defwKingValue[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};


static const int defwPawnValue[64] = { 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100
};

static const int defbPawnValue[64] = { 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100
};


static const int defbKnightValue[64] =
	{ 300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300
};

static const int defwKnightValue[64] =
	{ 300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300
};

static const int defbBishopValue[64] =
	{ 300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300
};

static const int defwBishopValue[64] =
	{ 300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300,
	300, 300, 300, 300, 300, 300, 300, 300
};

static const int defbRookValue[64] = { 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500
};

static const int defwRookValue[64] = { 500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500,
	500, 500, 500, 500, 500, 500, 500, 500
};


static const int defbQueenValue[64] =
	{ 900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900
};

static const int defwQueenValue[64] =
	{ 900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900,
	900, 900, 900, 900, 900, 900, 900, 900
};

static const int defbKingValue[64] = { 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

static const int safetyBonus[3][8] = 
{
	{ 30, 20, 10, 0, 0, 0, 0, 0 },
	{ 50, 30, 10, 0, 0, 0, 0, 0 },
	{ 30, 20, 10, 0, 0, 0, 0, 0 }	
};

static const int fianchettoSafetyBonus = 10;

static const int edgeSafetyBonus = 20;

/*static int piecevalues[12][128];*/
static int originalValues[MATING_PHASE][12][128];

/* piece taking value for ordering purposes [from][to] */
/*WKING, WQUEEN, WROOK, WBISHOP, WKNIGHT, WPAWN, BKING, BQUEEN, BROOK,
		BBISHOP,
	BKNIGHT, BPAWN*/
	
/* initialise pieces to the values specified in the file given.
Format is simply the sequence of values for the piecesquares going in the order
specified for the pieces in piecestruct.h, from square 0 to 127.
If file is NULL, load with default values */
int initialisePieceValues(char *finname)
{
	FILE *fin;
	pieceType i;
	int j,k;
	for (k = 0; k < MATING_PHASE; k++) {
		for (i = WKING; i < NOTHING; i++) {
			for (j = 0; j < 128; j++) {
				originalValues[k][i][j] = 0;
			}
		}
	}
	if (finname) {
		fin = fopen(finname, "r");
		for (k = 0; k < MATING_PHASE; k++) {		
			for (i = WKING; i < NOTHING; i++) {
				for (j = 0; j < 128; j++) {
					if (fscanf(fin, "%d", &originalValues[k][i][j]) != 1) {
						fprintf(stderr, "Error reading piece values\n");
						fclose(fin);
						return -1;
					}
				}
			}
		}
		fclose(fin);
	} else {
		for (k = 0; k < MATING_PHASE; k++) {
			for (j = 0; j < 64; j++) {
				originalValues[k][WKING][standardTox88(j)] = defwKingValue[j];
				originalValues[k][BKING][standardTox88(j)] = defbKingValue[j];
				originalValues[k][WPAWN][standardTox88(j)] = defwPawnValue[j];
				originalValues[k][BPAWN][standardTox88(j)] = defbPawnValue[j];
				originalValues[k][WBISHOP][standardTox88(j)] = defwBishopValue[j];
				originalValues[k][BBISHOP][standardTox88(j)] = defbBishopValue[j];
				originalValues[k][WROOK][standardTox88(j)] = defwRookValue[j];
				originalValues[k][BROOK][standardTox88(j)] = defbRookValue[j];
				originalValues[k][WKNIGHT][standardTox88(j)] = defwKnightValue[j];
				originalValues[k][BKNIGHT][standardTox88(j)] = defbKnightValue[j];
				originalValues[k][WQUEEN][standardTox88(j)] = defwQueenValue[j];
				originalValues[k][BQUEEN][standardTox88(j)] = defbQueenValue[j];
			}
		}
	}
	return 0;
}


void setPieceValues(Board *state) {
	int i,j, range, howmuch, phase;
	phase = state->phase;	
	howmuch = state->random;
	if (howmuch == 0) {
		for (i = WKING; i < NOTHING; i++) {
			for (j = 0; j < 64; j++) {
				state->piecevalues[i][standardTox88(j)] = originalValues[phase-1][i][standardTox88(j)];
			}
		}
	} else {
		range = howmuch * 2 + 1;
		for (i = WKING; i < NOTHING; i++) {
			for (j = 0; j < 64; j++) {
				state->piecevalues[i][standardTox88(j)] = originalValues[phase-1][i][standardTox88(j)] + 
				((random() % range) - howmuch);
			}
		}		
	}
}

int checkPhase(Board *board) {
	int changed = 0;
	if ((board->phase < MATING_PHASE) &&
	((board->piececount[WBISHOP] + board->piececount[WKNIGHT] +
		board->piececount[WROOK] + board->piececount[WQUEEN] +
		board->piececount[WPAWN] + board->piececount[BPAWN] == 0) ||
	(board->piececount[BBISHOP] + board->piececount[BKNIGHT] +
		board->piececount[BROOK] + board->piececount[BQUEEN] +
		board->piececount[BPAWN] + board->piececount[WPAWN] == 0))) {
		changed = 1;
		board->phase = MATING_PHASE;
		setPieceValues(board);	
	} else if ((board->phase < END_PHASE) && 
	(board->whitetotal + board->blacktotal < ENDPHASE_TOTAL)) {
		changed = 1;
		board->phase = END_PHASE;
		setPieceValues(board);
	}
	return changed;
}


inline static int distanceToPawn(const Board * const b, int position, int diff, const pieceType piece) {
	int i = 0;
	for (position += diff; !(position & OUT); position += diff) {
		if (b->squares[position] == piece) break;
		i++;
	}
	return i;
}

inline static int kingSafety(const Board * const b, int position, const int diff, const pieceType piece, 
    const pieceType bishopPiece) {
	int col, safety = 0, midSafety, midDistance;
	//pieceType bishopPiece;
	col = x88PosToCol(position);	
	if (col > 0) {
		safety += safetyBonus[0][distanceToPawn(b,position - 1, diff, piece)];
	} else {
		safety += edgeSafetyBonus;
	}
	midDistance = distanceToPawn(b,position, diff, piece);
	if ((midDistance == 1) && (b->squares[position + diff] == bishopPiece)) {
	    midSafety = safetyBonus[1][0] + fianchettoSafetyBonus;
	} else {
    	midSafety = safetyBonus[1][midDistance];	    
	}
	safety += midSafety;
	if (col < 7) {
		safety += safetyBonus[2][distanceToPawn(b,position + 1, diff, piece)];
	} else {
		safety += edgeSafetyBonus;
	}
	return safety;
}

static int kingSafetyDiff(const Board * const b) {
	int whiteSafety = 0, blackSafety = 0;
	
	if (b->phase == OPENING_PHASE) {
		whiteSafety = kingSafety(b,b->wkingpos,16, WPAWN, WBISHOP);
		blackSafety = kingSafety(b,b->bkingpos,-16, BPAWN, BBISHOP);
	}
	return whiteSafety - blackSafety;
}

int positionheuri(Board * board)
{
	int value, swhite, sblack, bwhite, bblack;
	board->totalevals++;

	if (board->piececount[WPAWN] + board->piececount[BPAWN] == 0) {
		/* check if it's a drawn position 
		(no big pieces and no more than one small piece each) */
		swhite = board->piececount[WBISHOP] + board->piececount[WKNIGHT];
		sblack = board->piececount[BBISHOP] + board->piececount[BKNIGHT];
		bwhite = board->piececount[WROOK] + board->piececount[WQUEEN];
		bblack = board->piececount[BROOK] + board->piececount[BQUEEN];
		if ((swhite < 2) && (sblack < 2) && (bwhite + bblack == 0)) {
			value = board->drawvalue;
		} else if (swhite + sblack == 0) {
			/* this little hack is to encourage simplification to mating phase */
			if (bwhite == 0) {
				value = board->whitetotal - (board->blacktotal << 1); /* bonus since it's winning */
			} else if (bblack == 0) {
				value = (board->whitetotal << 1) - board->blacktotal; /* bonus since it's winning */
			} else {
				value = board->whitetotal - board->blacktotal;
			}
		} 
		/* also assign draw to one side having only one rook, and the other one small piece */
		else if ((swhite == 0) && (bwhite == 1) && (sblack == 1) && (bblack == 0)) {
			if (board->piececount[WROOK] == 1) value = board->drawvalue;
			else value = board->whitetotal - board->blacktotal;
		} else if ((swhite == 1) && (bwhite == 0) && (sblack == 0) && (bblack == 1)) {
			if (board->piececount[BROOK] == 1) value = board->drawvalue;
			else value = board->whitetotal - board->blacktotal;
		} else {
			value = board->whitetotal - board->blacktotal;			
		} 
	} else {
		value = board->whitetotal - board->blacktotal + kingSafetyDiff(board);
	}

	return value;

}



int relativepositionheuri(Board * board)
{
	int wvalue, bvalue, value;
	board->totalevals++;
	if ((board->piececount[WPAWN] + board->piececount[BPAWN] == 0) && 
	(board->piececount[WBISHOP] + board->piececount[WKNIGHT] < 2) && 
	(board->piececount[BBISHOP] + board->piececount[BKNIGHT] < 2) &&
	(board->piececount[BROOK] + board->piececount[BQUEEN]  +
	 board->piececount[WROOK] + board->piececount[WQUEEN] == 0)) {
		value = board->drawvalue;
	} else {
		wvalue = board->whitetotal;
		bvalue = board->blacktotal;
		if (wvalue <= 0) {
			wvalue = 1;
		}
		if (bvalue <= 0) {
			bvalue = 1;
		}
		value = (int) (log((double) wvalue / (double) bvalue) * 10000);
	}

	return value;


}


