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
#include "board.h"
#include "pieces.h"
#include "hash.h"
#include "search.h"
#include "eval.h"

void setColorTotals(Board * board) {
	int i = 0, wvalue = 0, bvalue = 0;

	for (i = WKING; i < NOTHING; i++) {
		board->piececount[i] = 0;
	}
	i = 0;
	while (i <= H8) {
		if (board->squares[i] != NOTHING) board->piececount[board->squares[i]]++;
		if (board->colors[i] == BLACKP) bvalue += board->piecevalues[board->squares[i]][i];
		else if (board->colors[i] == WHITEP) wvalue += board->piecevalues[board->squares[i]][i];
		i++;
		if (i & OUT) i += 8;
	}
	board->blacktotal = bvalue;
	board->whitetotal = wvalue;
}

void initialiseBoard(Board * board)
{
	int i,j,k;
	for (i=0; i < 128; i++) {
		board->squares[i] = NOTHING;
		board->colors[i] = NOTHING;
	}
	board->squares[A1] = WROOK;
	board->squares[B1] = WKNIGHT;
	board->squares[C1] = WBISHOP;
	board->squares[D1] = WQUEEN;
	board->squares[E1] = WKING;
	board->squares[F1] = WBISHOP;
	board->squares[G1] = WKNIGHT;
	board->squares[H1] = WROOK;
	board->squares[A8] = BROOK;
	board->squares[B8] = BKNIGHT;
	board->squares[C8] = BBISHOP;
	board->squares[D8] = BQUEEN;
	board->squares[E8] = BKING;
	board->squares[F8] = BBISHOP;
	board->squares[G8] = BKNIGHT;
	board->squares[H8] = BROOK;
	for (i = A2; i <= H2; i++)
		board->squares[i] = WPAWN;
	for (i = A7; i <= H7; i++)
		board->squares[i] = BPAWN;
	for (i = A1; i <= H1; i++)
		board->colors[i] = WHITEP;
	for (i = A2; i <= H2; i++)
		board->colors[i] = WHITEP;
	for (i = A7; i <= H7; i++)
		board->colors[i] = BLACKP;
	for (i = A8; i <= H8; i++)
		board->colors[i] = BLACKP;
	for (i = 0; i < MAXPLYPERGAME; i++)
		board->history[i] = 0;
	board->palp[0] = NOPALP;
	board->ply = 0;
	board->fifty[0] = 0;
	board->wkingpos = E1;
	board->bkingpos = E8;
	board->blackturn = 0;
	board->wkcastle = board->wqcastle = board->bkcastle = board->bqcastle = 1;
	board->history[0] = getHashKey(board->hashtable, board);
	board->ourtime = board->theirtime = 0;
	board->totalnodes = board->totalevals =  board->totalhash = board->totalquiescent = 
	board->lastcheck = board->nulldepth = 0;
	board->anomaly = NORMAL;
	board->phase = OPENING_PHASE;
	setPieceValues(board);	
	setColorTotals(board);
	clearHashTable(board->hashtable);
	/* clear the history table */
	for (i = 0; i < 2; i++)
		for (j = 0; j < 128; j++)
			for (k = 0; k < 128; k++)
				board->betaers[i][j][k] = 0;
				
	for (i = 0; i <  MAXPLYPERGAME; i++) {
		for (j = 0; j < KILLERS; j++) {
			board->killers[i][j].move = NOMOVE;
			board->killers[i][j].score = 0;
		}
	}
	
}




void movePieceSimple(Board * const board, const Move * const move)
{
	pieceType frompiece, topiece;
	extMoveDescription *thismove;
	int thisply, nextply;
	
	thisply = board->ply;
	nextply = thisply + 1;
	
	frompiece = board->squares[move->from];
	topiece = board->squares[move->to];

	thismove = &board->game[thisply];
	thismove->from = move->from;
	thismove->to = move->to;
	thismove->frompiece = frompiece;
	thismove->topiece = topiece;	
	thismove->fromcolor = board->colors[move->from];
	thismove->tocolor = board->colors[move->to];
	thismove->flags = 0;

	board->squares[move->from] = NOTHING;
	board->colors[move->from] = NOTHING;

	if (board->blackturn) {
		if ((move->piece == NOTHING) || (frompiece != BPAWN)) {
			board->squares[move->to] = frompiece;
		} else {
			switch (move->piece) {
			case QUEEN:
				board->squares[move->to] = BQUEEN;
				break;
			case ROOK:
				board->squares[move->to] = BROOK;
				break;
			case BISHOP:
				board->squares[move->to] = BBISHOP;
				break;
			case KNIGHT:
				board->squares[move->to] = BKNIGHT;
				break;
			case PAWN:
				board->squares[move->to] = BPAWN;
				break;
			case KING:
				board->squares[move->to] = BKING;
				break;
			default:
				board->squares[move->to] = move->piece;
			}
		}
		if (topiece == WROOK) {	
			if ((move->to == A1) && (board->wqcastle)) {
				board->wqcastle = 0;
				thismove->flags |= WQCASTLEAVAIL;
			} else if ((move->to == H1) && (board->wkcastle)) {
				board->wkcastle = 0;
				thismove->flags |= WKCASTLEAVAIL;				
			}
		}

		board->colors[move->to] = BLACKP;
		if (frompiece == BPAWN) {
			board->fifty[nextply] = 0;
			if (move->to == move->from - 32) {
				board->palp[nextply] = move->from - 16;
			} else {
				if (move->to == board->palp[thisply]) {
					board->squares[move->to + 16] = NOTHING;
					board->colors[move->to + 16] = NOTHING;
					thismove->flags |= PALP;
				}
				board->palp[nextply] = NOPALP;
			}
		} else {
			board->palp[nextply] = NOPALP;
			if (topiece != NOTHING) {
				board->fifty[nextply] = board->fifty[thisply] + 1;
			} else {
				board->fifty[nextply] = 0;
			}
			if (frompiece == BROOK) {
				if ((move->from == A8) && (board->bqcastle)) {
					board->bqcastle = 0;
					thismove->flags |= BQCASTLEAVAIL;					
				} else if ((move->from == H8) && (board->bkcastle)) {
					board->bkcastle = 0;
					thismove->flags |= BKCASTLEAVAIL;					
				}
			} else if (move->from == board->bkingpos) {
				board->bkingpos = move->to;
				if (board->bqcastle) {
					board->bqcastle = 0;
					thismove->flags |= BQCASTLEAVAIL;
				}
				if (board->bkcastle) {
					board->bkcastle = 0;
					thismove->flags |= BKCASTLEAVAIL;					
				}
				
				if (move->from == E8) {
					if (move->to == G8) {
						thismove->flags |= BKCASTLE;
						board->squares[H8] = NOTHING;
						board->squares[F8] = BROOK;
						board->colors[H8] = NOTHING;
						board->colors[F8] = BLACKP;

					} else {
						if (move->to == C8) {
							thismove->flags |= BQCASTLE;
							board->squares[A8] = NOTHING;
							board->squares[D8] = BROOK;
							board->colors[A8] = NOTHING;
							board->colors[D8] = BLACKP;
						}
					}
				}
			}
		}
		board->blackturn = 0;
	} else {
		if ((move->piece == NOTHING) || (frompiece != WPAWN)) {
			board->squares[move->to] = frompiece;
		} else {
			switch (move->piece) {
			case QUEEN:
				board->squares[move->to] = WQUEEN;
				break;
			case ROOK:
				board->squares[move->to] = WROOK;
				break;
			case BISHOP:
				board->squares[move->to] = WBISHOP;
				break;
			case KNIGHT:
				board->squares[move->to] = WKNIGHT;
				break;
			case PAWN:
				board->squares[move->to] = WPAWN;
				break;
			case KING:
				board->squares[move->to] = WKING;
				break;
			default:
				board->squares[move->to] = move->piece;
			}

		}
		if (topiece == BROOK) {
			if ((move->to == A8) && (board->bqcastle)) {
				board->bqcastle = 0;
				thismove->flags |= BQCASTLEAVAIL;	
			} else if ((move->to == H8) && (board->bkcastle)) {
				board->bkcastle = 0;
				thismove->flags |= BKCASTLEAVAIL;					
			}
		}


		board->colors[move->to] = WHITEP;
		if (frompiece == WPAWN) {
			board->fifty[nextply] = 0;
			if (move->to == move->from + 32) {
				board->palp[nextply] = move->from + 16;
			} else {
				if (move->to == board->palp[thisply]) {
					board->squares[move->to - 16] = NOTHING;
					board->colors[move->to - 16] = NOTHING;
					thismove->flags |= PALP;
				}

				board->palp[nextply] = NOPALP;
			}
		} else {
			board->palp[nextply] = NOPALP;
			if (topiece != NOTHING) {
				board->fifty[nextply] = board->fifty[thisply] + 1;
			} else {
				board->fifty[nextply] = 0;
			}

			if (frompiece == WROOK) {	
				if ((move->from == A1) && (board->wqcastle)) {
					board->wqcastle = 0;
					thismove->flags |= WQCASTLEAVAIL;						
				} else if ((move->from == H1) && (board->wkcastle)) {
					board->wkcastle = 0;
					thismove->flags |= WKCASTLEAVAIL;					
				}
			} else if (move->from == board->wkingpos) {
				board->wkingpos = move->to;
				if (board->wqcastle) {
					board->wqcastle = 0;
					thismove->flags |= WQCASTLEAVAIL;					
				}
				if (board->wkcastle) {
					board->wkcastle = 0;
					thismove->flags |= WKCASTLEAVAIL;					
				}
				
				if (move->from == E1) {
					if (move->to == G1) {
						thismove->flags |= WKCASTLE;
						board->squares[H1] = NOTHING;
						board->squares[F1] = WROOK;
						board->colors[H1] = NOTHING;
						board->colors[F1] = WHITEP;

					} else {
						if (move->to == C1) {
							thismove->flags |= WQCASTLE;
							board->squares[A1] = NOTHING;
							board->squares[D1] = WROOK;
							board->colors[A1] = NOTHING;
							board->colors[D1] = WHITEP;
						}
					}
				}
			}
		}
		board->blackturn = 1;
	}
	board->ply++;

}


void
movePiece(Board * const board, const Move * const move)
{
	pieceType frompiece, topiece;
	extMoveDescription *thismove;
	int thisply, nextply;
	U64 key;
	int wking;

	
	thisply = board->ply;
	nextply = thisply + 1;

	wking = (int) WKING;

	key = board->history[thisply];	/* ^ board->hashtable->blackhash; */
	frompiece = board->squares[move->from];
	key ^= board->hashtable->piecekeys[frompiece - wking][move->from];
	key ^= board->hashtable->piecekeys[NOTHING - wking][move->from];
	topiece = board->squares[move->to];
	key ^= board->hashtable->piecekeys[topiece - wking][move->to];
	if (board->palp[thisply] >= 0) {
		key ^= board->hashtable->piecekeys[13][board->palp[thisply]];
	}

	thismove = &board->game[thisply];
	thismove->from = move->from;
	thismove->to = move->to;
	thismove->frompiece = frompiece;
	thismove->topiece = topiece;	
	thismove->fromcolor = board->colors[move->from];
	thismove->tocolor = board->colors[move->to];
	thismove->flags = 0;
	
	board->squares[move->from] = NOTHING;
	board->colors[move->from] = NOTHING;	

	if (board->blackturn) {
		if ((move->piece == NOTHING) || (frompiece != BPAWN)) {
			board->squares[move->to] = frompiece;
			board->blacktotal += board->piecevalues[frompiece][move->to] - 
			board->piecevalues[frompiece][move->from];
		} else {
			switch (move->piece) {
			case QUEEN:
				board->squares[move->to] = BQUEEN;
				break;
			case ROOK:
				board->squares[move->to] = BROOK;
				break;
			case BISHOP:
				board->squares[move->to] = BBISHOP;
				break;
			case KNIGHT:
				board->squares[move->to] = BKNIGHT;
				break;
			case PAWN:
				board->squares[move->to] = BPAWN;
				break;
			case KING:
				board->squares[move->to] = BKING;
				break;
			default:
				board->squares[move->to] = move->piece;
			}
			board->blacktotal += board->piecevalues[board->squares[move->to]][move->to] -
			board->piecevalues[BPAWN][move->from];
			board->piececount[BPAWN]--;
			board->piececount[board->squares[move->to]]++;
		}
		if (topiece == WROOK) {	
			if ((move->to == A1) && (board->wqcastle)) {
				board->wqcastle = 0;
				key ^= board->hashtable->wqcastle;
				thismove->flags |= WQCASTLEAVAIL;				
			} else if ((move->to == H1) && (board->wkcastle)) {
				board->wkcastle = 0;
				key ^= board->hashtable->wkcastle;
				thismove->flags |= WKCASTLEAVAIL;								
			}
		}
		board->colors[move->to] = BLACKP;
		if (frompiece == BPAWN) {
			board->fifty[nextply] = 0;
			if (move->to == move->from - 32) {
				board->palp[nextply] = move->from - 16;
				key ^= board->hashtable->piecekeys[13][board->palp[nextply]];
			} else {
				if (move->to == board->palp[thisply]) {
					board->whitetotal -= board->piecevalues[WPAWN][move->to + 16];
					board->piececount[WPAWN]--;
					board->squares[move->to + 16] = NOTHING;
					board->colors[move->to + 16] = NOTHING;
					key ^=
						board->hashtable->piecekeys[WPAWN - wking]
											[move->to + 16] ^
						board->hashtable->piecekeys[NOTHING - wking]
											[move->to + 16];
					thismove->flags |= PALP;
				} else if (topiece != NOTHING) {
					board->whitetotal -= board->piecevalues[topiece][move->to];
					board->piececount[topiece]--;
				}
				board->palp[nextply] = NOPALP;
			}
		} else {
			board->palp[nextply] = NOPALP;

			if (topiece == NOTHING) {
				board->fifty[nextply] = board->fifty[thisply] + 1;
			} else {
				board->piececount[topiece]--;
				board->whitetotal -= board->piecevalues[topiece][move->to];				
				board->fifty[nextply] = 0;
			}
			if (frompiece == BROOK) {
				if ((move->from == A8) && (board->bqcastle)) {
					board->bqcastle = 0;
					key ^= board->hashtable->bqcastle;
					thismove->flags |= BQCASTLEAVAIL;	
				} else if ((move->from == H8) && (board->bkcastle)) {
					board->bkcastle = 0;
					key ^= board->hashtable->bkcastle;
					thismove->flags |= BKCASTLEAVAIL;						
				}
			} else if (move->from == board->bkingpos) {
				board->bkingpos = move->to;
				if (board->bqcastle) {
					board->bqcastle = 0;
					key ^= board->hashtable->bqcastle;
					thismove->flags |= BQCASTLEAVAIL;						
				}
				if (board->bkcastle) {
					board->bkcastle = 0;
					key ^= board->hashtable->bkcastle;
					thismove->flags |= BKCASTLEAVAIL;						
				}

				if (move->from == E8) {
					if (move->to == G8) {
						board->squares[H8] = NOTHING;
						board->squares[F8] = BROOK;
						board->colors[H8] = NOTHING;
						board->colors[F8] = BLACKP;
						board->blacktotal += board->piecevalues[BROOK][F8] - 
						board->piecevalues[BROOK][H8];
						thismove->flags |= BKCASTLE;	
						key ^=
							board->hashtable->piecekeys[BROOK - wking][H8] ^
							board->hashtable->piecekeys[BROOK - wking][F8] ^
							board->hashtable->piecekeys[NOTHING - wking][F8] ^
							board->hashtable->piecekeys[NOTHING - wking][H8];
					} else {
						if (move->to == C8) {
							board->squares[A8] = NOTHING;
							board->squares[D8] = BROOK;
							board->colors[A8] = NOTHING;
							board->colors[D8] = BLACKP;
							board->blacktotal += board->piecevalues[BROOK][D8] - 
							board->piecevalues[BROOK][A8];							
							thismove->flags |= BQCASTLE;
							key ^=
								board->hashtable->piecekeys[BROOK - wking][D8] ^
								board->hashtable->piecekeys[BROOK - wking][A8] ^
								board->hashtable->piecekeys[NOTHING - wking][D8] ^
								board->hashtable->piecekeys[NOTHING - wking][A8];

						}
					}
				}
			}
		}
		board->blackturn = 0;
	} else {
				
		if ((move->piece == NOTHING) || (frompiece != WPAWN)) {
			board->squares[move->to] = frompiece;
			board->whitetotal += board->piecevalues[frompiece][move->to] - 
			board->piecevalues[frompiece][move->from];
		} else {
			switch (move->piece) {
			case QUEEN:
				board->squares[move->to] = WQUEEN;
				break;
			case ROOK:
				board->squares[move->to] = WROOK;
				break;
			case BISHOP:
				board->squares[move->to] = WBISHOP;
				break;
			case KNIGHT:
				board->squares[move->to] = WKNIGHT;
				break;
			case PAWN:
				board->squares[move->to] = WPAWN;
				break;
			case KING:
				board->squares[move->to] = WKING;
				break;
			default:
				board->squares[move->to] = move->piece;
			}
			board->whitetotal += board->piecevalues[board->squares[move->to]][move->to] -
			board->piecevalues[WPAWN][move->from];
			board->piececount[WPAWN]--;
			board->piececount[board->squares[move->to]]++;
		}

		if (topiece == BROOK) {
			if ((move->to == A8) && (board->bqcastle)) {
				board->bqcastle = 0;
				key ^= board->hashtable->bqcastle;
				thismove->flags |= BQCASTLEAVAIL;
			} else if ((move->to == H8) && (board->bkcastle)) {
				board->bkcastle = 0;
				key ^= board->hashtable->bkcastle;
				thismove->flags |= BKCASTLEAVAIL;				
			}
		}
		board->colors[move->to] = WHITEP;
		if (frompiece == WPAWN) {
			board->fifty[nextply] = 0;
			if (move->to == move->from + 32) {
				board->palp[nextply] = move->from + 16;
				key ^= board->hashtable->piecekeys[13][board->palp[nextply]];
			} else {
				if (move->to == board->palp[thisply]) {
					board->blacktotal -= board->piecevalues[BPAWN][move->to - 16];					
					board->piececount[BPAWN]--;					
					board->squares[move->to - 16] = NOTHING;
					board->colors[move->to - 16] = NOTHING;
					key ^=
						board->hashtable->piecekeys[BPAWN - wking]
							[move->to - 16] ^
						board->hashtable->piecekeys[NOTHING - wking]
											[move->to - 16];
					thismove->flags |= PALP;
				} else if (topiece != NOTHING) {
					board->blacktotal -= board->piecevalues[topiece][move->to];
					board->piececount[topiece]--;					
				}

				board->palp[nextply] = NOPALP;
			}
		} else {
			board->palp[nextply] = NOPALP;
			if (topiece == NOTHING) {
				board->fifty[nextply] = board->fifty[thisply] + 1;
			} else {
				board->piececount[topiece]--;				
				board->blacktotal -= board->piecevalues[topiece][move->to];				
				board->fifty[nextply] = 0;
			}
			if (frompiece == WROOK) {	
				if ((move->from == A1) && (board->wqcastle)) {
					board->wqcastle = 0;
					key ^= board->hashtable->wqcastle;
					thismove->flags |= WQCASTLEAVAIL;
				} else if ((move->from == H1) && (board->wkcastle)) {
					board->wkcastle = 0;
					key ^= board->hashtable->wkcastle;
					thismove->flags |= WKCASTLEAVAIL;
				}
			} else if (move->from == board->wkingpos) {
				board->wkingpos = move->to;
				if (board->wqcastle) {
					board->wqcastle = 0;
					key ^= board->hashtable->wqcastle;
					thismove->flags |= WQCASTLEAVAIL;
				}
				if (board->wkcastle) {
					board->wkcastle = 0;
					key ^= board->hashtable->wkcastle;
					thismove->flags |= WKCASTLEAVAIL;
				}

				if (move->from == E1) {
					if (move->to == G1) {
						board->squares[H1] = NOTHING;
						board->squares[F1] = WROOK;
						board->colors[H1] = NOTHING;
						board->colors[F1] = WHITEP;
						board->whitetotal += board->piecevalues[WROOK][F1] - 
						board->piecevalues[WROOK][H1];						
						thismove->flags |= WKCASTLE;
						key ^=
							board->hashtable->piecekeys[WROOK -
																		wking][H1] ^
							board->hashtable->piecekeys[WROOK -
																		wking][F1] ^
							board->hashtable->piecekeys[NOTHING -
																		wking][H1] ^
							board->hashtable->piecekeys[NOTHING - wking][F1];
							
					} else {
						if (move->to == C1) {
							board->squares[A1] = NOTHING;
							board->squares[D1] = WROOK;
							board->colors[A1] = NOTHING;
							board->colors[D1] = WHITEP;
							board->whitetotal += board->piecevalues[WROOK][D1] - 
							board->piecevalues[WROOK][A1];
							thismove->flags |= WQCASTLE;	
							key ^=
								board->hashtable->piecekeys[WROOK -
																			wking][D1] ^
								board->hashtable->piecekeys[WROOK -
																			wking][A1] ^
								board->hashtable->piecekeys[NOTHING -
																			wking][D1] ^
								board->hashtable->piecekeys[NOTHING - wking][A1];

						}
					}
				}

			}

		}
		board->blackturn = 1;
	}
	key ^=
		board->hashtable->piecekeys[board->squares[move->to] -
													wking][move->to];

	board->history[nextply] = key;
	board->ply++;

}

void unMove(Board * const board) {
	extMoveDescription *thismove;
	pieceType currpiece;
	
	board->blackturn = 1 - board->blackturn;

	thismove = &board->game[board->ply - 1];
	currpiece = board->squares[thismove->to];
	board->squares[thismove->from] = thismove->frompiece;
	board->colors[thismove->from] = thismove->fromcolor;
	board->squares[thismove->to] = thismove->topiece;
	board->colors[thismove->to] = thismove->tocolor;

	if (thismove->flags & BQCASTLEAVAIL) {
		board->bqcastle = 1;
	}
	if (thismove->flags & BKCASTLEAVAIL) {
		board->bkcastle = 1;
	}
	if (thismove->flags & WQCASTLEAVAIL) {
		board->wqcastle = 1;
	}
	if (thismove->flags & WKCASTLEAVAIL) {
		board->wkcastle = 1;
	}

	if (currpiece != thismove->frompiece) {
		board->piececount[thismove->frompiece]++;
		board->piececount[currpiece]--;
	}

	if (board->blackturn) {
		board->blacktotal += board->piecevalues[thismove->frompiece][thismove->from] -
		board->piecevalues[currpiece][thismove->to];
		if (thismove->topiece != NOTHING) {
			board->whitetotal += board->piecevalues[thismove->topiece][thismove->to];
			board->piececount[thismove->topiece]++;
		}
		if (thismove->frompiece == BKING) {
			board->bkingpos = thismove->from;
			if (thismove->flags & BKCASTLE) {
				board->squares[H8] = BROOK;
				board->squares[F8] = NOTHING;
				board->colors[H8] = BLACKP;
				board->colors[F8] = NOTHING;	
				board->blacktotal += board->piecevalues[BROOK][H8] - 
				board->piecevalues[BROOK][F8];				
			} else if (thismove->flags & BQCASTLE) {
				board->squares[A8] = BROOK;
				board->squares[D8] = NOTHING;				
				board->colors[A8] = BLACKP;
				board->colors[D8] = NOTHING;				
				board->blacktotal += board->piecevalues[BROOK][A8] - 
				board->piecevalues[BROOK][D8];				
			}
		} else if (thismove->flags & PALP) {
			board->whitetotal += board->piecevalues[WPAWN][thismove->to + 16];
			board->piececount[WPAWN]++;
			board->squares[thismove->to + 16] = WPAWN;
			board->colors[thismove->to + 16] = WHITEP;
		}
	} else {
		board->whitetotal += board->piecevalues[thismove->frompiece][thismove->from] -
		board->piecevalues[currpiece][thismove->to];
		if (thismove->topiece != NOTHING) {
			board->blacktotal += board->piecevalues[thismove->topiece][thismove->to];
			board->piececount[thismove->topiece]++;			
		}		
		if (thismove->frompiece == WKING) {
			board->wkingpos = thismove->from;
			if (thismove->flags & WKCASTLE) {
				board->squares[H1] = WROOK;
				board->squares[F1] = NOTHING;				
				board->colors[H1] = WHITEP;
				board->colors[F1] = NOTHING;				
				board->whitetotal += board->piecevalues[WROOK][H1] - 
				board->piecevalues[WROOK][F1];						
			} else if (thismove->flags & WQCASTLE) {
				board->squares[A1] = WROOK;
				board->squares[D1] = NOTHING;				
				board->colors[A1] = WHITEP;
				board->colors[D1] = NOTHING;				
				board->whitetotal += board->piecevalues[WROOK][A1] - 
				board->piecevalues[WROOK][D1];										
			}
			
		} else if (thismove->flags & PALP) {
			board->piececount[BPAWN]++;			
			board->blacktotal += board->piecevalues[BPAWN][thismove->to - 16];			
			board->squares[thismove->to - 16] = BPAWN;
			board->colors[thismove->to - 16] = BLACKP;
		}

	}

	board->ply--;
}

void unMoveSimple(Board * const board) {
	extMoveDescription *thismove;
	
	board->blackturn = 1 - board->blackturn;

	thismove = &board->game[board->ply - 1];
	board->squares[thismove->from] = thismove->frompiece;
	board->colors[thismove->from] = thismove->fromcolor;
	board->squares[thismove->to] = thismove->topiece;
	board->colors[thismove->to] = thismove->tocolor;

	if (thismove->flags & BQCASTLEAVAIL) {
		board->bqcastle = 1;
	}
	if (thismove->flags & BKCASTLEAVAIL) {
		board->bkcastle = 1;
	}
	if (thismove->flags & WQCASTLEAVAIL) {
		board->wqcastle = 1;
	}
	if (thismove->flags & WKCASTLEAVAIL) {
		board->wkcastle = 1;
	}

	if (board->blackturn) {
		if (thismove->frompiece == BKING) {
			board->bkingpos = thismove->from;
			if (thismove->flags & BKCASTLE) {
				board->squares[H8] = BROOK;
				board->squares[F8] = NOTHING;
				board->colors[H8] = BLACKP;
				board->colors[F8] = NOTHING;	
			} else if (thismove->flags & BQCASTLE) {
				board->squares[A8] = BROOK;
				board->squares[D8] = NOTHING;				
				board->colors[A8] = BLACKP;
				board->colors[D8] = NOTHING;				
			}
		} else if (thismove->flags & PALP) {
			board->squares[thismove->to + 16] = WPAWN;
			board->colors[thismove->to + 16] = WHITEP;
		}
	} else {
		if (thismove->frompiece == WKING) {
			board->wkingpos = thismove->from;
			if (thismove->flags & WKCASTLE) {
				board->squares[H1] = WROOK;
				board->squares[F1] = NOTHING;				
				board->colors[H1] = WHITEP;
				board->colors[F1] = NOTHING;				
			} else if (thismove->flags & WQCASTLE) {
				board->squares[A1] = WROOK;
				board->squares[D1] = NOTHING;				
				board->colors[A1] = WHITEP;
				board->colors[D1] = NOTHING;				
			}
			
		} else if (thismove->flags & PALP) {
			board->squares[thismove->to - 16] = BPAWN;
			board->colors[thismove->to - 16] = BLACKP;
		}

	}
	board->ply--;
}

void makeNullMove(Board * const board) {
	int thisply, nextply;
			
	thisply = board->ply;
	nextply = thisply + 1;
	board->history[nextply] = board->history[thisply];	/* ^ board->hashtable->blackhash; */
	board->blackturn = 1 - board->blackturn;	
	board->palp[nextply] = NOPALP;
	board->fifty[nextply] = board->fifty[thisply] + 1;
	board->game[thisply].flags = NULLMOVE;
	board->ply++;
}

void unmakeNullMove(Board * const board) {
	board->blackturn = 1 - board->blackturn;	
	board->ply--;	
}


/*  Returns 0 if the player whose move it is cannot move, !0 if it can */
int canMove(Board * const board)
{
	Move movelist[MAXMOVESPERNODE];
	int numbermoves;
	
	numbermoves = generateMoveList(board, movelist);
	return (numbermoves > 0);
}

/* Returns 0 if the player whose move it is is not in check, 1 if it is */
int inCheck(const Board * const board)
{
	int kingpos, i, result, j, incheck, oppkingpos, total, current,
		blocked, diffkings, currrow, currcol;
	int *moves;
	pieceType currpiece;

	result = incheck = blocked = 0;
	if (board->blackturn) {
		kingpos = board->bkingpos;
		oppkingpos = board->wkingpos;
		currrow = x88PosToRow(kingpos);
		currcol = x88PosToCol(kingpos);

		moves = knightMoves[kingpos];
		for (i = 0;
			  ((moves[i] != -1) && (board->squares[moves[i]] != WKNIGHT)); i++);
		if (moves[i] == -1) {
			total = currcol;
			current = kingpos - 1;
			for (j = 0; (j < total) && (incheck == 0) && (blocked == 0); j++) {
				currpiece = board->squares[current];
				if ((currpiece == NOTHING) ||
					 (currpiece == WROOK) || (currpiece == WQUEEN)) {
					if (currpiece != NOTHING) {
						incheck = 1;
					} else {
						current--;
					}
				} else {
					blocked = 1;
				}
			}
			if (incheck == 0) {
				blocked = 0;
				total = 7 - currcol;
				current = kingpos + 1;
				for (j = 0; (j < total) && (incheck == 0) && (blocked == 0); j++) {
					currpiece = board->squares[current];
					if ((currpiece == NOTHING) ||
						 (currpiece == WROOK) || (currpiece == WQUEEN)) {
						if (currpiece != NOTHING) {
							incheck = 1;
						} else {
							current++;
						}
					} else {
						blocked = 1;
					}
				}
				if (incheck == 0) {
					blocked = 0;
					total = currrow;
					current = kingpos - 16;
					for (j = 0; (j < total) && (incheck == 0) && (blocked == 0);
						  j++) {
						currpiece = board->squares[current];
						if ((currpiece == NOTHING) ||
							 (currpiece == WROOK) || (currpiece == WQUEEN)) {
							if (currpiece != NOTHING) {
								incheck = 1;
							} else {
								current -= 16;
							}
						} else {
							blocked = 1;
						}
					}
					if (incheck == 0) {
						blocked = 0;
						total = 7 - currrow;
						current = kingpos + 16;
						for (j = 0; (j < total) && (incheck == 0)
							  && (blocked == 0); j++) {
							currpiece = board->squares[current];
							if ((currpiece == NOTHING) ||
								 (currpiece == WROOK) || (currpiece == WQUEEN)) {
								if (currpiece != NOTHING) {
									incheck = 1;
								} else {
									current += 16;
								}
							} else {
								blocked = 1;
							}
						}
					}
					if (incheck == 0) {
						blocked = 0;
						if (currcol < currrow) {
							total = currcol;
						} else {
							total = currrow;
						}
						current = kingpos - 17;
						for (j = 0; (j < total) && (incheck == 0)
							  && (blocked == 0); j++) {
							currpiece = board->squares[current];
							if ((currpiece == NOTHING) ||
								 (currpiece == WBISHOP) || (currpiece == WQUEEN)) {
								if (currpiece != NOTHING) {
									incheck = 1;
								} else {
									current -= 17;
								}
							} else {
								blocked = 1;
							}
						}
						if (incheck == 0) {
							blocked = 0;
							if (7 - currcol < currrow) {
								total = 7 - currcol;
							} else {
								total = currrow;
							}
							current = kingpos - 15;
							for (j = 0; (j < total) && (incheck == 0)
								  && (blocked == 0); j++) {
								currpiece = board->squares[current];
								if ((currpiece == NOTHING) || (currpiece == WBISHOP)
									 || (currpiece == WQUEEN)) {
									if (currpiece != NOTHING) {
										incheck = 1;
									} else {
										current -= 15;
									}
								} else {
									blocked = 1;
								}
							}
							if (incheck == 0) {
								blocked = 0;
								if (currcol < 7 - currrow) {
									total = currcol;
								} else {
									total = 7 - currrow;
								}
								current = kingpos + 15;
								for (j = 0; (j < total) && (incheck == 0)
									  && (blocked == 0); j++) {
									currpiece = board->squares[current];
									if ((currpiece == NOTHING)
										 || (currpiece == WBISHOP)
										 || (currpiece == WQUEEN)) {
										if (currpiece != NOTHING) {
											incheck = 1;
										} else {
											current += 15;
										}
									} else {
										blocked = 1;
									}
								}
								if (incheck == 0) {
									blocked = 0;
									if (currcol < currrow) {
										total = 7 - currrow;
									} else {
										total = 7 - currcol;
									}
									current = kingpos + 17;
									for (j = 0; (j < total) && (incheck == 0)
										  && (blocked == 0); j++) {
										currpiece = board->squares[current];
										if ((currpiece == NOTHING)
											 || (currpiece == WBISHOP)
											 || (currpiece == WQUEEN)) {
											if (currpiece != NOTHING) {
												incheck = 1;
											} else {
												current += 17;
											}
										} else {
											blocked = 1;
										}
									}

									if (incheck == 0) {

										if ((currrow >= 2)
											 && (((currcol > 0)
													&& (board->squares[kingpos - 17] ==
														 WPAWN)) || ((currcol < 7)
																		 && (board->squares
																			  [kingpos - 15] ==
																			  WPAWN)))) {
											incheck = 1;
										} else {
											diffkings = oppkingpos - kingpos;
											if (((diffkings <= 17) && (diffkings >= -17)) && 
												(((diffkings == -1)
												  && (currcol > 0))
												 || ((diffkings == 1)
													  && (currcol < 7))
												 || ((diffkings == 16)
													  && (currrow < 7))
												 || ((diffkings == -16)
													  && (currrow > 0))
												 || ((diffkings == -17)
													  && (currrow > 0)
													  && (currcol > 0))
												 || ((diffkings == -15)
													  && (currrow > 0)
													  && (currcol < 7))
												 || ((diffkings == 15)
													  && (currrow < 7)
													  && (currcol > 0))
												 || ((diffkings == 17)
													  && (currrow < 7)
													  && (currcol < 7)))) {
												incheck = 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			result = incheck;
		} else {
			result = 1;
		}
	} else {
		kingpos = board->wkingpos;
		oppkingpos = board->bkingpos;
		currrow = x88PosToRow(kingpos);
		currcol = x88PosToCol(kingpos);

		moves = knightMoves[kingpos];
		for (i = 0;
			  ((moves[i] != -1) && (board->squares[moves[i]] != BKNIGHT)); i++);
		if (moves[i] == -1) {
			total = currcol;
			current = kingpos - 1;
			for (j = 0; (j < total) && (incheck == 0) && (blocked == 0); j++) {
				currpiece = board->squares[current];
				if ((currpiece == NOTHING) ||
					 (currpiece == BROOK) || (currpiece == BQUEEN)) {
					if (currpiece != NOTHING) {
						incheck = 1;
					} else {
						current--;
					}
				} else {
					blocked = 1;
				}
			}
			if (incheck == 0) {
				blocked = 0;
				total = 7 - currcol;
				current = kingpos + 1;
				for (j = 0; (j < total) && (incheck == 0) && (blocked == 0); j++) {
					currpiece = board->squares[current];
					if ((currpiece == NOTHING) ||
						 (currpiece == BROOK) || (currpiece == BQUEEN)) {
						if (currpiece != NOTHING) {
							incheck = 1;
						} else {
							current++;
						}
					} else {
						blocked = 1;
					}
				}
				if (incheck == 0) {
					blocked = 0;
					total = currrow;
					current = kingpos - 16;
					for (j = 0; (j < total) && (incheck == 0) && (blocked == 0);
						  j++) {
						currpiece = board->squares[current];
						if ((currpiece == NOTHING) ||
							 (currpiece == BROOK) || (currpiece == BQUEEN)) {
							if (currpiece != NOTHING) {
								incheck = 1;
							} else {
								current -= 16;
							}
						} else {
							blocked = 1;
						}
					}
					if (incheck == 0) {
						blocked = 0;
						total = 7 - currrow;
						current = kingpos + 16;
						for (j = 0; (j < total) && (incheck == 0)
							  && (blocked == 0); j++) {
							currpiece = board->squares[current];
							if ((currpiece == NOTHING) ||
								 (currpiece == BROOK) || (currpiece == BQUEEN)) {
								if (currpiece != NOTHING) {
									incheck = 1;
								} else {
									current += 16;
								}
							} else {
								blocked = 1;
							}
						}
					}
					if (incheck == 0) {
						blocked = 0;
						if (currcol < currrow) {
							total = currcol;
						} else {
							total = currrow;
						}
						current = kingpos - 17;
						for (j = 0; (j < total) && (incheck == 0)
							  && (blocked == 0); j++) {
							currpiece = board->squares[current];
							if ((currpiece == NOTHING) ||
								 (currpiece == BBISHOP) || (currpiece == BQUEEN)) {
								if (currpiece != NOTHING) {
									incheck = 1;
								} else {
									current -= 17;
								}
							} else {
								blocked = 1;
							}
						}
						if (incheck == 0) {
							blocked = 0;
							if (7 - currcol < currrow) {
								total = 7 - currcol;
							} else {
								total = currrow;
							}
							current = kingpos - 15;
							for (j = 0; (j < total) && (incheck == 0)
								  && (blocked == 0); j++) {
								currpiece = board->squares[current];
								if ((currpiece == NOTHING) || (currpiece == BBISHOP)
									 || (currpiece == BQUEEN)) {
									if (currpiece != NOTHING) {
										incheck = 1;
									} else {
										current -= 15;
									}
								} else {
									blocked = 1;
								}
							}
							if (incheck == 0) {
								blocked = 0;
								if (currcol < 7 - currrow) {
									total = currcol;
								} else {
									total = 7 - currrow;
								}
								current = kingpos + 15;
								for (j = 0; (j < total) && (incheck == 0)
									  && (blocked == 0); j++) {
									currpiece = board->squares[current];
									if ((currpiece == NOTHING)
										 || (currpiece == BBISHOP)
										 || (currpiece == BQUEEN)) {
										if (currpiece != NOTHING) {
											incheck = 1;
										} else {
											current += 15;
										}
									} else {
										blocked = 1;
									}
								}
								if (incheck == 0) {
									blocked = 0;
									if (currcol < currrow) {
										total = 7 - currrow;
									} else {
										total = 7 - currcol;
									}
									current = kingpos + 17;
									for (j = 0; (j < total) && (incheck == 0)
										  && (blocked == 0); j++) {
										currpiece = board->squares[current];
										if ((currpiece == NOTHING)
											 || (currpiece == BBISHOP)
											 || (currpiece == BQUEEN)) {
											if (currpiece != NOTHING) {
												incheck = 1;
											} else {
												current += 17;
											}
										} else {
											blocked = 1;
										}
									}

									if (incheck == 0) {

										if ((currrow <= 5)
											 && (((currcol > 0)
													&& (board->squares[kingpos + 15] ==
														 BPAWN)) || ((currcol < 7)
																		 && (board->squares
																			  [kingpos + 17] ==
																			  BPAWN)))) {
											incheck = 1;
										} else {
											diffkings = oppkingpos - kingpos;
											if (((diffkings <= 17) && (diffkings >= -17)) && 
												(((diffkings == -1)
												  && (currcol > 0))
												 || ((diffkings == 1)
													  && (currcol < 7))
												 || ((diffkings == 16)
													  && (currrow < 7))
												 || ((diffkings == -16)
													  && (currrow > 0))
												 || ((diffkings == -17)
													  && (currrow > 0)
													  && (currcol > 0))
												 || ((diffkings == -15)
													  && (currrow > 0)
													  && (currcol < 7))
												 || ((diffkings == 15)
													  && (currrow < 7)
													  && (currcol > 0))
												 || ((diffkings == 17)
													  && (currrow < 7)
													  && (currcol < 7)))) {
												incheck = 1;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			result = incheck;
		} else {
			result = 1;
		}
	}

	return result;
}

/* Returns 0 if side to move is not in check, !0 if it is*/
int leftInCheck(Board *board, Move *move) 
{
	int incheck;

	movePieceSimple(board, move);
	board->blackturn = 1 - board->blackturn;
	incheck = inCheck(board);
	board->blackturn = 1 - board->blackturn;
	unMoveSimple(board);		
	return incheck;
}

/* Returns 0 if the move is not legal, !0 if it is legal*/
int isLegal(Board * board, Move * move)
{
	int incheck;
	if (pieceCanMove(board, move->from, move->to)) {
		movePieceSimple(board, move);
		board->blackturn = 1 - board->blackturn;
		incheck = inCheck(board);
		board->blackturn = 1 - board->blackturn;
		unMoveSimple(board);		
		return !incheck;
	} else {
		return 0;
	}
}

int isCheckmate(Board * board)
{
	int result;
	result = 0;
	if (inCheck(board)) {
		if (canMove(board) == 0) {
			result = 1;
		}
	}
	return result;

}

int isStalemate(Board * board)
{
	int result;
	result = 0;
	if (inCheck(board) == 0) {
		if (canMove(board) == 0) {
			result = 1;
		}
	}
	return result;
}

int isRepetitionDraw(Board * board)
{
	int count, i, value, firstpossible;
	U64 key;
	if (board->fifty[board->ply] < 100) {
		key = board->history[board->ply];
		count = 0;
		firstpossible = board->ply - board->fifty[board->ply];		
		for (i = board->ply - 2; (i >= firstpossible) && (count < 2); i -= 2) {
			if (board->history[i] == key)
				count++;
		}
		if (count < 2) {
			value = 0;
		} else {
			value = 1;
		}
	} else {
		value = 1;
	}
	return value;
}

/* check if it's an enforceable draw by insufficient material
(maximum one minor piece on the whole board) */
int isInsufficientMaterialDraw(Board * board) {
	int value = 0;
	if (board->piececount[WPAWN] + board->piececount[BPAWN] +
	board->piececount[WQUEEN] + board->piececount[BQUEEN] +
	board->piececount[WROOK] + board->piececount[BROOK] == 0) {
		if (board->piececount[WBISHOP] + board->piececount[BBISHOP] +
		board->piececount[BKNIGHT] + board->piececount[WKNIGHT] <= 1) value = 1;
	}
	return value;
}

/* check if position has been repeated before (just once.  this is for evaluation, not
declaration) */
int checkRepetitionDraw(Board * board)
{
	int i, firstpossible;
	U64 key;
	if (board->fifty[board->ply] < 100) {
		key = board->history[board->ply];
		firstpossible = board->ply - board->fifty[board->ply];
		for (i = board->ply - 2; (i >= firstpossible) && (board->history[i] != key);
			  i -= 2);
		return (i >= firstpossible);
	} else {
		return 1;
	}
}


/* Generate a list of legal moves, store them in movelist, return number of moves*/
int generateMoveList(Board * const b, Move *movelist) {
	int movecount = 0, from = 0, newmovecount = 0, ok = 1, oppking, legalcount;
	Move *tempmove;
	pieceType color;
	
	if (b->blackturn) {
		color = BLACKP;
		oppking = b->wkingpos;		
	} else {
		color = WHITEP;
		oppking = b->bkingpos;		
	}
	
	for (; (from < I8) && (b->colors[from] != color); from++);
	while ((from < I8) && (ok)) {
		newmovecount = generateMoves[b->squares[from]](b,from,movelist,movecount);
		/* check if attacking opposite king */
		for (; (movecount < newmovecount) && (ok); movecount++) {
			ok = (movelist[movecount].to != oppking);
		}
		for (from++; (b->colors[from] != color) && (from < I8); from++);		
	}
	
	if (ok) {
		/*check if moves are legal */
		for (tempmove = movelist, legalcount = 0; tempmove < &(movelist[movecount]); tempmove++) {
			if (!leftInCheck(b, tempmove)) {
				movelist[legalcount++] = *tempmove;
			}
		}
	} else {
		legalcount = -1;
	}
	return legalcount;
}


/* Generate a list of legal moves to be tried on quiescent search, store them in 
movelist, return number of moves.
For now, quiescent moves are all moves that take a piece */
int generateQuiescentMoveList(Board * const b, Move *movelist) {
	int movecount = 0, from = 0, newmovecount = 0, ok = 1, oppking, legalcount;
	Move *tempmove;
	pieceType color;
	
	if (b->blackturn) {
		color = BLACKP;
		oppking = b->wkingpos;		
	} else {
		color = WHITEP;
		oppking = b->bkingpos;		
	}
	
	for (; (from < I8) && (b->colors[from] != color); from++);
	while ((from < I8) && (ok)) {
		newmovecount = generateCaptures[b->squares[from]](b,from,movelist,movecount);
		/* check if attacking opposite king */
		for (; (movecount < newmovecount) && (ok); movecount++) {
			ok = (movelist[movecount].to != oppking);
		}
		for (from++; (b->colors[from] != color) && (from < I8); from++);		
	}
	
	if (ok) {
		/*check if moves are legal */
		for (tempmove = movelist, legalcount = 0; tempmove < &(movelist[movecount]); tempmove++) {
			if (!leftInCheck(b, tempmove)) {
				movelist[legalcount++] = *tempmove;
			}
		}
	} else {
		legalcount = -1;
	}
	return legalcount;
}

/* Generate a list of possibly legal moves (maybe leaving the player in check), 
store them in movelist, return number of moves.  Return -1 if taking oppositions
king is an option (illegal position) */
int generateTentativeMoveList(const Board * const b, Move *movelist) {
	int movecount = 0, from = 0, newmovecount = 0, ok = 1, oppking;
	pieceType color;
	
	if (b->blackturn) {
		color = BLACKP;
		oppking = b->wkingpos;		
	} else {
		color = WHITEP;
		oppking = b->bkingpos;		
	}
	
	for (; (from < I8) && (b->colors[from] != color); from++);
	while ((from < I8) && (ok)) {
		newmovecount = generateMoves[b->squares[from]](b,from,movelist,movecount);
		/* check if attacking opposite king */
		for (; (movecount < newmovecount) && (ok); movecount++) {
			ok = (movelist[movecount].to != oppking);
		}
		for (from++; (b->colors[from] != color) && (from < I8); from++);		
	}
	
	if (!ok) {
		movecount = -1;
	}
	return movecount;
}

/* Generate a list of possibly legal moves (maybe leaving the player in check) to be 
tried on quiescent search, store them in 
movelist, return number of moves. Return -1 if taking oppositions
king is an option (illegal position)
For now, quiescent moves are all moves that take a piece */
int generateTentativeQuiescentMoveList(const Board * const b, Move *movelist) {
	int movecount = 0, from = 0, newmovecount = 0, oppking;
	pieceType color;
	
	if (b->blackturn) {
		color = BLACKP;
		oppking = b->wkingpos;		
	} else {
		color = WHITEP;
		oppking = b->bkingpos;		
	}
	
	/* Find the first piece on our side.
	 * Assume there is a piece on our side, because otherwise our king was taken at some point prior, and it would have been picked up */
	while (1) {
		if (b->colors[from] == color) break;
		if (++from & OUT) {
			from += 8;
		}
	}
	while (1) {
		newmovecount = generateCaptures[b->squares[from]](b,from,movelist,movecount);
		/* check if attacking opposite king */
		while (movecount < newmovecount) {
			if  (movelist[movecount++].to == oppking)
			{
				movecount = -1;
				goto outOfLoop;
			}
		}
		while (1) {
			if (++from & OUT) {
				if (from >= I8) goto outOfLoop;
				from += 8;
			}
			if (b->colors[from] == color) break;
		}
	}
	outOfLoop:
	return movecount;
}
