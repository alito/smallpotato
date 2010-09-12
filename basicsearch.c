/*
   Copyright (C) 2001 Alejandro Dubrovsky
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
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
#include "search.h"
#include "eval.h"
#include "pieces.h"
#include "board.h"
#include "utils.h"
#include <stdlib.h>
#include <sys/time.h>

int randomPlay(Board * b, Move * move)
{
	int numbermoves;
	Move movelist[MAXMOVESPERNODE];	
	numbermoves = generateMoveList(b,movelist);	
	*move = movelist[(int) (random() % numbermoves)];
	return NORMAL;
}


int oneStep(Board * b, Move * move)
{
	int best = MINVALUE - 1, current, canmove, draw, moveindex, numbermoves, incheck;
	pieceType color;
	Move tempmove, currbest;
	Move movelist[MAXMOVESPERNODE];	
		
	if (b->blackturn) {
		color = BLACKP;
	} else {
		color = WHITEP;
	}

	numbermoves = generateMoveList(b,movelist);
	if (numbermoves == 1) {
		/* if there's only one possible move, return it immediately */
		*move = movelist[0];
		if (!(b->actionmode & ANALYSEMODE)) {
			return NORMAL;
		} else {
			currbest = movelist[0];
			best = 0;
		}
	} else {			
		moveindex = 0;
		while (moveindex < numbermoves) {
			tempmove = movelist[moveindex++];
			draw = 0;
			movePiece(b, &tempmove);
			canmove = canMove(b);
			if (canmove == 0) {
				incheck = inCheck(b);
				if (incheck) {
					*move = tempmove;
					/* checkmate in one */
					if (!(b->actionmode & ANALYSEMODE))	{
						unMove(b);
						return NORMAL;
					} else {
						current = MAXVALUE - 1;
					}
				} else {
					/* stalemate */
					current = 0;
					draw = 1;
				}
			} else {
				if (b->fifty[b->ply] >= 100) {
					current = 0;
					draw = 1;
				} else {
					if (isRepetitionDraw(b)) {
						current = 0;
						draw = 1;
					} else {
						current = (*(b->evalboard))
							(b);
						if (color == BLACKP) {
							current = -current;
						}
					}
				}
			}

			unMove(b);
			
			if (current > best) {
				best = current;
				currbest = tempmove;
			}
		}
	}
	*move = currbest;
	return NORMAL;
}

static int negamax(Board * b, int depth)
{
	pieceType color;
	Move tempmove;
	Move movelist[MAXMOVESPERNODE];	
	int best, current, canmove, incheck, numbermoves, moveindex,draw;
	if (b->blackturn) {
		color = BLACKP;
	} else {
		color = WHITEP;
	}
	
	best = -2 * KINGVALUE;
	
	numbermoves = generateMoveList(b,movelist);

	moveindex = 0;
	while (moveindex < numbermoves) {
		tempmove = movelist[moveindex++];
		draw = 0;
		movePiece(b, &tempmove);	
		canmove = canMove(b);
		if (canmove == 0) {
			incheck = inCheck(b);
			if (incheck) {
				unMove(b);
				return 2 * KINGVALUE;

			} else {
				unMove(b);
				return 0;
			}
		} else {
			if (b->fifty[b->ply] >= 100) {
				unMove(b);
				return 0;
			} else {

				if (depth > 1) {
					current =
						-negamax(b, depth - 1);
				} else {
					current = (*(b->evalboard)) (b);
					if (color == BLACKP) {
						current = -current;
					}

				}
				if (current > best) {
					best = current;
				}
			}
		}
		unMove(b);
	}
	return best;

}


int multiNegamax(Board * b, Move * move)
{
	pieceType color;
	Move tempmove;
	Move movelist[MAXMOVESPERNODE];		
	FILE *logfile;
	char buffer[10];
	int best, current, levels, startnodes, endnodes,
		canmove, incheck, numbermoves, moveindex, draw;
	if (b->blackturn) {
		color = BLACKP;
	} else {
		color = WHITEP;
	}
	best = -2 * KINGVALUE;
	logfile = b->logfile;
	levels = b->depth;
	if (levels == 0) {
		levels = 4;
	}
	startnodes = b->totalnodes;
	numbermoves = generateMoveList(b,movelist);

	moveindex = 0;
	while (moveindex < numbermoves) {
		tempmove = movelist[moveindex++];
		draw = 0;
		movePiece(b, &tempmove);	
		canmove = canMove(b);
		if (canmove == 0) {
			incheck = inCheck(b);
			if (incheck) {
				unMove(b);
				return 2 * KINGVALUE;

			} else {
				unMove(b);
				return 0;
			}
		} else {
			if (b->fifty[b->ply] >= 100) {
				unMove(b);
				return 0;
			} else {

				if (levels > 1) {
					current =
						-negamax(b, levels - 1);
				} else {
					current = (*(b->evalboard)) (b);
					if (color == BLACKP) {
						current = -current;
					}

				}
				if (current > best) {
					best = current;
					*move = tempmove;
				}
			}
		}
		unMove(b);
	}
	if (b->verbosity > 0) {
		endnodes = b->totalnodes;
		moveToString(move, buffer);
		fprintf(logfile, "level %d\t", levels);
		fprintf(logfile, "level nodes %d\t", endnodes - startnodes);
		fprintf(logfile, "best score %d\t", best);
		fprintf(logfile, "move %s\n", buffer);
	}
	return NORMAL;
}



static int
alphabeta(Board * b, int depth, int alpha)
{
	pieceType color;
	int best, current, canmove, incheck, numbermoves, moveindex, draw;
	Move tempmove;
	Move movelist[MAXMOVESPERNODE];		
	incheck = inCheck(b);
	canmove = canMove(b);

	if ((incheck != 0) && (canmove == 0)) {
		if (b->blackturn)
			return KINGVALUE + depth;
		else
			return -KINGVALUE - depth;
	} else {

		if (((incheck == 0) && (canmove == 0)) || (b->fifty[b->ply] >= 100))
			return 0;
		else {
			if (depth == 0)
				return (*(b->evalboard)) (b);
			else {
				if (b->blackturn) {
					color = BLACKP;
					best = 2 * KINGVALUE;
				} else {
					color = WHITEP;
					best = -2 * KINGVALUE;
				}
				numbermoves = generateMoveList(b,movelist);
			
				moveindex = 0;
				while (moveindex < numbermoves) {
					tempmove = movelist[moveindex++];
					draw = 0;
					movePiece(b, &tempmove);	

					current =
						alphabeta(b, depth - 1,
									 best);
					if (((color == BLACKP) && (current < best))
						 || ((color == WHITEP) && (current > best))) {
						best = current;

					}
					unMove(b);
					if (((color == BLACKP) && (best <= alpha)) ||
						 ((color == WHITEP) && (best >= alpha))) {
						return best;
					}
				}
				return best;
			}
		}
	}
}



int multiStep(Board * b, Move * move)
{
	pieceType color;
	Move tempmove;
	Move movelist[MAXMOVESPERNODE];		
	int best, current, levels, numbermoves, moveindex, draw;
	if (b->blackturn) {
		color = BLACKP;
		best = 2 * KINGVALUE;
	} else {
		color = WHITEP;
		best = -2 * KINGVALUE;
	}
	move->piece = NOTHING;
	levels = b->depth;
	if (levels == 0) {
		levels = 4;
	}
	numbermoves = generateMoveList(b,movelist);

	moveindex = 0;
	while (moveindex < numbermoves) {
		tempmove = movelist[moveindex++];
		draw = 0;
		movePiece(b, &tempmove);	
		current =
			alphabeta(b, levels - 1, best);
		if (((color == BLACKP) && (current < best))
			 || ((color == WHITEP) && (current > best))) {
			best = current;
			*move = tempmove;
		}
		unMove(b);

	}
	return NORMAL;
}
