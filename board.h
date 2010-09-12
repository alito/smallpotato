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

#ifndef BOARD_H
#define BOARD_H 1

#include "common.h"
#include "piecesstruct.h"
#include "communication.h"
#include "hashstruct.h"

#define MAXPLYPERGAME 500
#define MAXEPDMOVES 5
#define KILLERS 2

#define WKCASTLE 1
#define WQCASTLE 1 << 1
#define BKCASTLE 1 << 2
#define BQCASTLE 1 << 3
#define WKCASTLEAVAIL 1 << 4
#define WQCASTLEAVAIL 1 << 5
#define BKCASTLEAVAIL 1 << 6
#define BQCASTLEAVAIL 1 << 7
#define NULLMOVE 1 << 8
#define PALP 1 << 8
#define NORMALMODE 1
#define BOOKMODE 1 << 1
#define FORCEMODE 1 << 2
#define ANALYSEMODE 1 << 3
#define PONDERMODE 1 << 4
#define NORMAL 1
#define ACT 1 << 1
#define MOVE_AND_ACT 1 << 2
#define WAIT_FOR_EVENT 1 << 3
#define MOVE_AND_RESIGN 1 << 4
#define OPENING_PHASE 1
#define END_PHASE 2
#define MATING_PHASE 3

/* Squares (stolen and modified from Phalanx XXII  (didn't feel like typing) */

#define A1 0
#define B1 1
#define C1 2
#define D1 3
#define E1 4
#define F1 5
#define G1 6
#define H1 7

#define A2 16
#define B2 17
#define C2 18
#define D2 19
#define E2 20
#define F2 21
#define G2 22
#define H2 23

#define A3 32
#define B3 33
#define C3 34
#define D3 35
#define E3 36
#define F3 37
#define G3 38
#define H3 39

#define A4 48
#define B4 49
#define C4 50
#define D4 51
#define E4 52
#define F4 53
#define G4 54
#define H4 55

#define A5 64
#define B5 65
#define C5 66
#define D5 67
#define E5 68
#define F5 69
#define G5 70
#define H5 71

#define A6 80
#define B6 81
#define C6 82
#define D6 83
#define E6 84
#define F6 85
#define G6 86
#define H6 87

#define A7 96
#define B7 97
#define C7 98
#define D7 99
#define E7 100
#define F7 101
#define G7 102
#define H7 103

#define A8 112
#define B8 113
#define C8 114
#define D8 115
#define E8 116
#define F8 117
#define G8 118
#define H8 119
#define I8 120

#define OUT 0x88
#define NOPALP -100

struct epdMoveStruct {
	int avoid;   /*1 if to be avoided, 0 if to be sought */
	int count;  /* number of epdmoves */
	Move moves[MAXEPDMOVES];	
};

typedef struct epdMoveStruct epdMove;

struct extMoveDescriptionStruct {
	int from, to;
	pieceType frompiece, topiece, fromcolor, tocolor;
	unsigned int flags;
};

typedef struct extMoveDescriptionStruct extMoveDescription;

struct pvStruct {
	int length;
	Move line[MAXPLYPERGAME];
};

typedef struct pvStruct PV;

struct killerStruct {
	Move move;
	int score;
};

typedef struct killerStruct Killer;

struct boardStruct
{
	timeControl timecontrol;
	hashTable *hashtable; /* transposition table */
	FILE *logfile, *gamefile; /* file for log, and for game recording */
	prodcons *input, *output; /* input and output queues */
	int (*evalboard) ();  /* evaluation function to use */
	int (*engine) ();  /* search engine to use */
	int (*searcher) ();  /* searching bit (MTDf, negascout) */
	int betaers[2][128][128]; /* history list of beta-cutting moves */
	int piecevalues[NOTHING - WKING][128]; /* piece-value tables for current phase */
	int piececount[NOTHING - WKING]; /* number of pieces of each type left*/
	PV pv; /* the principal variation */
	char *name;  /* engine name */
	epdMove epdmove;  /* the epdmove/s to look for/avoid */
	extMoveDescription game[MAXPLYPERGAME]; /* history of moves, so that we can unmove */
	pieceType squares[128]; /* piece on each square */
	pieceType colors[128]; /* color of each square */
	Action lastaction;
	Killer killers[MAXPLYPERGAME][KILLERS]; /* killer moves */
	U64 history[MAXPLYPERGAME];  /* hash of board at each ply */
	int fifty[MAXPLYPERGAME];  /* number of plys since last pawn was moved, or piece taken at each ply*/
	int palp[MAXPLYPERGAME];  /* en-passant numbers at each ply */
	int blacktotal, whitetotal; /* total value for black and white (by piecevalues above) */
	int wkcastle, bkcastle, wqcastle, bqcastle; /* rights to each castle */
	int blackturn;  /* 1 for black's turn, 0 for not */
	int ply;  /* ply number of the game */
	int bkingpos, wkingpos;  /* position of white and black king */
	int ourtime;  /* hundredths of seconds on our clock */
	int theirtime; /* hundredths of seconds on their clock */	
	int totalnodes, totalevals, totalhash, totalquiescent; /* general counters */
	int lastcheck;  /* when last time check was made */
	int post;  /* post thinking */
	int analyse;  /* analyse mode */
	int depth;  /* maximum depth to search */
	int verbosity;  /* how much are we throwing to the log */
	int record; /* are we recording the game */
	int resign; /* evaluation at which we should resign */
	int timelimit;  /* maximum number of hundredths of seconds per move */
	int random;	 /* how much noise to add to the value tables before each game */
	int actionmode; /* what are we doing */
	int ponder; /* do we ponder if we can */
	int anomaly; /* as defined above (NORMAL, ACT, MOVE_AND_ACT) */
	int phase; /* what phase of the game we are in (eg opening, middle, endgame) */
	int nulldepth; /* denotes last depth at which nullmove was taken */
	int epdmindepth;  /* don't return from an epd until at least this depth has been reached */	
	int drawvalue; /* value of a draw */
	int timeaggression; /* a number to denote how much time to take on each move, which at
	present represents the minimum percentage of time of an "average move" */
};

typedef struct boardStruct Board;

void setColorTotals(Board * board);
void initialiseBoard(Board * board);

void movePieceSimple(Board * board, const Move * const move);
void movePiece(Board * board, const Move * const move);
void unMove(Board * const board);
void unMoveSimple(Board * const board);

void makeNullMove(Board * const board);
void unmakeNullMove(Board * const board);

int leftInCheck(Board *board, Move *move);
int isLegal(Board * board, Move * move);
int inCheck(const Board * const board);
int isCheckmate(Board * board);
int isStalemate(Board * board);
int isRepetitionDraw(Board * board);
int isInsufficientMaterialDraw(Board * board);
int generateMoveList(Board * const b, Move *movelist);
int generateTentativeMoveList(const Board * const b, Move *movelist);
int generateQuiescentMoveList(Board * const b, Move *movelist);
int generateTentativeQuiescentMoveList(const Board * const b, Move *movelist);
int checkRepetitionDraw(Board * board);
int canMove(Board * const board);

inline int x88ToStandard(const int x88);
inline int standardTox88(const int standard);
inline int x88PosToRow(const int pos);
inline int x88PosToCol(const int pos);
inline int posToRow(const int pos);
inline int posToCol(const int pos);
inline int rowColToPos(const int row, const int col);
inline int x88RowColToPos(const int row, const int col);
#endif
