/*
   Copyright (C) 2002 Alejandro Dubrovsky
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
#include "search.h"
#include "eval.h"
#include "hash.h"
#include "moveorder.h"
#include "utils.h"
#include "board.h"
#include "pieces.h"
#include "book.h"
#include "ai.h"
#include <libale.h>
#include <sys/time.h>
#include <inttypes.h>


/* patched into the verynewalphabeta code templating from 
MTD(f) - A Minimax Algorithm faster than NegaScout, 
by Aske Plaat, and then fixed from his PhD Thesis */
 
int negaScout ( Board *b, Move *move, int depth, int alpha, int beta, Flags *flags )
{                        
	U64 key, position;
	hashChunk *hashchunk;
	hashTable *hashtable;
	int best = MINVALUE - 1, current = 0,  tempalpha, bestwhen = 0, numbermoves = 0, moveindex = 0, gottime = 1, tempbeta, allillegal = 1;
	int moveorder[MAXMOVESPERNODE]; 
	Move tempmove, sugmove, bestmove, submove;
	Move movelist[MAXMOVESPERNODE];
	Action inaction;
	FILE *logfile;
	struct timeval temptime;
	
	b->totalnodes++;
	hashtable = b->hashtable;
	logfile = b->logfile;
	
	key = b->history[b->ply];
	position = key % hashtable->size;


	if (b->blackturn) {
		hashchunk = &hashtable->black[position];
	} else {
		hashchunk = &hashtable->white[position];
	}

	bestmove = sugmove = NOMOVE;


	/* see if position is stored in hashtable */
	if (flags->signal & FLAG_SIGNAL_DONTHASH) {
		flags->signal ^= FLAG_SIGNAL_DONTHASH;
	} else {
		if (hashchunk->depth > 0) {
			if (hashchunk->key == key) {
				int hiok, lowok;
				b->totalhash++;
				if (hashchunk->depth >= 100) {
					/*  100 moves and can still get the same hash.  That's a draw */
					return 0;
				}
				if (hashchunk->flags == HASH_ACCURATE) {
					hiok = lowok = 1;
				} else if (hashchunk->flags == HASH_LOWBOUND) {
					lowok = 1;
					hiok = 0;
				} else if (hashchunk->flags == HASH_UPBOUND) {
					hiok = 1;
					lowok = 0;
				} else {
					fprintf(logfile,
							  "hash value wasn't marked as accurate, lowerbound or upperbound\n");
					hiok = lowok = 0;
				}
				if (hashchunk->depth >= depth) {
					if ((hiok) && (lowok)) {
						if (b->verbosity > 15) {
							fprintf(logfile,
									  "hashed exact level %d\t value %d\t hash key %" PRIu64 "\n",
									  hashchunk->depth, hashchunk->lbound, key);
						}
						move->from = hashchunk->from;
						move->to = hashchunk->to;
						move->piece = (pieceType) hashchunk->piece;
						if (hashchunk->lbound >= MAXVALUE - MAXPLYPERGAME) {
							return hashchunk->lbound + hashchunk->ply - b->ply;
						}
						if (hashchunk->lbound <= MINVALUE + MAXPLYPERGAME) {
							return hashchunk->lbound - hashchunk->ply + b->ply;
						}
						return hashchunk->lbound;
					}
					if ((hashchunk->lbound >= beta) && (lowok)) {
						if (b->verbosity > 15) {
							fprintf(logfile,
									  "hashed beta cut off level %d\t value %d\t hash key %" PRIu64 "\n",
									  hashchunk->depth, hashchunk->lbound, key);
						}
						move->from = hashchunk->from;
						move->to = hashchunk->to;
						move->piece = (pieceType) hashchunk->piece;
						if (hashchunk->lbound >= MAXVALUE - MAXPLYPERGAME) {
							return hashchunk->lbound + hashchunk->ply - b->ply;
						}						
						return hashchunk->lbound;
					}
					if ((hashchunk->ubound <= alpha) && (hiok)) {
						if (b->verbosity > 15) {
							fprintf(logfile,
									  "hashed alpha cut off level %d\t value %d\thash key %" PRIu64 "\n",
									  hashchunk->depth, hashchunk->ubound, key);
						}
						move->from = hashchunk->from;
						move->to = hashchunk->to;
						move->piece = (pieceType) hashchunk->piece;
						if (hashchunk->ubound <= MINVALUE + MAXPLYPERGAME) {
							return hashchunk->ubound - hashchunk->ply + b->ply;
						}						
						return hashchunk->ubound;
					}

					if ((lowok) && (hashchunk->lbound > alpha)) {
						if (b->verbosity > 15) {
							fprintf(logfile,
									  "modified alpha by hash level %d\t value %d\thash key %" PRIu64 "\n",
									  hashchunk->depth, hashchunk->lbound, key);
						}
						if (hashchunk->lbound >= MAXVALUE - MAXPLYPERGAME) {
							alpha = hashchunk->lbound + hashchunk->ply - b->ply;
						} else {
							alpha = hashchunk->lbound;
						}
						/* should update the best move, maybe */
						bestmove.from = hashchunk->from;
						bestmove.to = hashchunk->to;
						bestmove.piece = (pieceType) hashchunk->piece;
						best = alpha;
					}
					if ((hiok) && (hashchunk->ubound < beta)) {
						if (b->verbosity > 15) {
							fprintf(logfile,
									  "modified beta by hash level %d\t value %d\thash key %" PRIu64 "\n",
									  hashchunk->depth, hashchunk->ubound, key);
						}
						if (hashchunk->ubound <= MINVALUE + MAXPLYPERGAME) {
							beta = hashchunk->ubound - hashchunk->ply + b->ply;
						} else {
							beta = hashchunk->ubound;
						}
					}
	
				}
				sugmove.from = hashchunk->from;
				sugmove.to = hashchunk->to;
				sugmove.piece = (pieceType) hashchunk->piece;
				if (b->verbosity > 15) {
					fprintf(logfile,
							  "hashed suggested move level %d\t value %d\thash key %" PRIu64 "\n",
							  hashchunk->depth, hashchunk->lbound, key);
				}
	
			} else {
				if (b->verbosity > 15) {
					fprintf(logfile, "hash collision\thash key %" PRIu64 "\n", key);
					if (b->verbosity > 30) {
						if (key != getHashKey(hashtable,b)) {
							fprintf(logfile, "HASH KEYS DO NOT MATCH\n");
						}
						printBoard(logfile,b);
					}
				}
			}
		}
	}
	tempalpha = alpha;
	tempbeta = beta;

	
	if (gottime) {		 
		numbermoves = generateTentativeMoveList(b,movelist);
		
		if (numbermoves > 1) {
			/* Before ordering, try null-move search.  We have to do this after the move
			generation to check if this is a legal position.
			Do null if the last move leading to this depth wasn't a null move,
			there are major pieces left (to avoid zugzwang-laden positions),
			we are not searching a PV-node,
			and we are not in mating phase */			
			int lastnull = b->nulldepth;
			if (((lastnull == 0) || (lastnull > depth + DEPTH_REDUCTION + 1)) && 
			(b->piececount[BROOK] + b->piececount[BQUEEN]  +
			 b->piececount[WROOK] + b->piececount[WQUEEN] > 0) &&
			(tempbeta - tempalpha == 1) &&
			(b->phase < MATING_PHASE)) {
				int canttouchme;
				makeNullMove(b);
				b->nulldepth = depth;
				if (depth > DEPTH_REDUCTION + 1) {
					canttouchme = -negaScout(b, &submove, depth - DEPTH_REDUCTION - 1, 
					-tempbeta, -tempalpha, flags);			
				} else {
					canttouchme = -quiescentSearch(b, -tempbeta, -tempalpha, flags);
				}
				b->nulldepth = lastnull;
				unmakeNullMove(b);
				if (canttouchme == ILLEGAL_POSITION) {
					/* we are in check */
					;
				} else if (canttouchme >= tempbeta) {
					return canttouchme;
				}
				if (flags->signal & FLAG_SIGNAL_OUTOFTIME) {
					gottime = 0;
				}					
			}
			orderMoveList(b,movelist,numbermoves,moveorder,&sugmove);
		} else if (numbermoves == 1) {
			moveorder[0] = 0;
		} else if (numbermoves < 0) {
			b->totalnodes--;
			if (b->verbosity > 15) {
				fprintf(logfile, "illegal position\n");
			}
			return ILLEGAL_POSITION;
		} else { 
		/* numbermoves is zero, can't move, stalemate or checkmate */
			if (inCheck(b)) return MINVALUE + b->ply;
			else return 0;
		}
		if (b->verbosity > 25) {
			fprintf(logfile,"Move order: ");
			for (moveindex = 0; moveindex < numbermoves; moveindex++) {
				fprintf(logfile,"%d ",moveorder[moveindex]);
			}
			fprintf(logfile,"\n");
		}
	}
	
	if (gottime) {
		moveindex = 0;
		while ((moveindex < numbermoves) && (best < beta) && (gottime)) {
			tempmove = movelist[moveorder[moveindex++]];
			movePiece(b, &tempmove);
			if (b->fifty[b->ply] >= 100) {
				/* FIXME: what if there's checkmate and 100 ply at the same time? */
				/* do a quick check to see if the move we just did put us in check */
				current = -quiescentSearch(b, -tempbeta, -tempalpha, flags);
				if (current != -ILLEGAL_POSITION) {
					current = b->drawvalue;
					allillegal = 0;
				}
			} else {
				if (checkRepetitionDraw(b)) {
					current = b->drawvalue;
					allillegal = 0;
				} else {
					if (depth > 1) {
						/* clear killer moves at depth + 1 to avoid killers lingering
						from far away, irrelevant nodes */
						clearKillers(b,b->ply + 1);
						current = -negaScout(b, &submove, depth - 1, -tempbeta, -tempalpha, flags);
						if (current != -ILLEGAL_POSITION) {
							allillegal = 0;
							/*
							if (depth == 2) {
								tempalpha = current;
							}*/

						}
						
					} else {
						b->totalnodes++;
						b->totalquiescent--;
						current = -quiescentSearch(b, -tempbeta, -tempalpha, flags);
						if (current == -ILLEGAL_POSITION) {
							b->totalnodes--;
							b->totalquiescent++;
						} else allillegal = 0;
					}
					if (flags->signal & FLAG_SIGNAL_OUTOFTIME) {
						gottime = 0;
					} else if ((current > tempalpha) && (current < beta) && (moveindex > 0)) {
						int tempcurrent;
						if (depth > 1) {
							tempcurrent = -negaScout(b, &submove, depth - 1, -beta, -current, flags);
						} else {
							b->totalnodes++;
							b->totalquiescent--;
							tempcurrent = -quiescentSearch(b, -beta, -current, flags);
						}
						if (flags->signal & FLAG_SIGNAL_OUTOFTIME) {
							gottime = 0;
						} else {
							if (tempcurrent > current) current = tempcurrent;
						}
					}
				}
			}
	
			unMove(b);
			if (gottime) {
				if (current > best) {
					bestwhen = moveindex;
					best = current;
					bestmove = tempmove;
					if (best > tempalpha) {
						tempalpha = best;
					}
				}
				tempbeta = tempalpha + 1;
				if (b->totalnodes + b->totalquiescent > b->lastcheck + NODES_PER_CHECK) {
					b->lastcheck = b->totalnodes + b->totalquiescent;
					gettimeofday(&temptime,NULL);
					if ((timercmp(&temptime,&(flags->stoptime),>)) && (!(b->actionmode & ANALYSEMODE))) {
						flags->signal |= FLAG_SIGNAL_OUTOFTIME;
						gottime = 0;
					} else {
						int emptyqueue = tryGetAction(b->input, &inaction);
						if (!emptyqueue) {
							handleActionWhileInSearch(b, &inaction, flags);
							if (flags->signal & FLAG_SIGNAL_OUTOFTIME) gottime = 0;
						}
					}
				}
			}		
		}
	}
	
	if (gottime) {
		int noderesult;
		*move = bestmove;
		if (allillegal) {
			/* can't move, stalemate or checkmate */
			if (inCheck(b)) {
				/*	printBoard(logfile,b); */
				return MINVALUE + b->ply;
			}
			else return b->drawvalue;			
		}
		if (best >= beta) {
			noderesult = BETACUT;
			/* update killer table */
			updateKiller(b,&bestmove);
			/* adjust history table */
			b->betaers[b->blackturn][bestmove.from][bestmove.to] += depth * depth;
			if (b->betaers[b->blackturn][bestmove.from][bestmove.to] > MAXHISTORYVALUE) {
				makeBetaersSmaller(b);
			}						
		}	else {
			if (best <= alpha) {
				noderesult = BELOWALPHA;
			} else {
			/* if ((best > alpha) && (best < beta)) {*/
				/* update killer table */
				updateKiller(b,&bestmove);			
				/* adjust history table */		
				b->betaers[b->blackturn][bestmove.from][bestmove.to] += depth * depth;		
				if (b->betaers[b->blackturn][bestmove.from][bestmove.to] > MAXHISTORYVALUE) {
    				makeBetaersSmaller(b);
				}										
				noderesult = NOCUT;			
			}
		}
	
		/* store evaluation and best moves of current b into hashtable if either
		there was nothing on that position, it is the same position as this, but
		analysed to a lower depth, or is a position whose reach was lower than this one,
		or this is a proper result for the node and what we are replacing isn't*/
		if ((hashchunk->ply + hashchunk->depth < b->ply + depth)
			 || ((hashchunk->depth < depth) && (hashchunk->key == key))
			 || ((noderesult == NOCUT) && (hashchunk->flags != HASH_ACCURATE))
			 || (hashchunk->depth < 0)) {
			 	
			if (noderesult == BETACUT) {
				hashchunk->lbound = best;
				hashchunk->flags = HASH_LOWBOUND;
			}	else {
				if (noderesult == BELOWALPHA) {
					hashchunk->ubound = best;
					hashchunk->flags = HASH_UPBOUND;			
				} else {
				/* if ((best > alpha) && (best < beta)) 
					not reached in mtdf*/
					hashchunk->ubound = hashchunk->lbound = best;
					hashchunk->flags = HASH_ACCURATE;	
				}
			}
	
	
			hashchunk->ply = (short int) b->ply;
			hashchunk->key = key;
			hashchunk->depth = depth;
			hashchunk->from = (unsigned char) bestmove.from;
			hashchunk->to = (unsigned char) bestmove.to;
			hashchunk->piece = (char) bestmove.piece;
			
			
		}
	}
	if ((b->verbosity > 8) && (depth > 3)) {
		fprintf(logfile, "checked %d at depth %d\n", bestwhen, depth);
		if (!gottime) {
			fprintf(logfile, "out of time\n");
		}
	}
	return best;

   
}

/* aspiration search, two steps.  first step search with small window,
if not in window, go to full width */
int negaScoutDriver(Board * b, Move * move,
					 int depth, int value, Flags *flags)
{
	int thevalue, alpha, beta, windowwidth = 40;
	alpha = value - windowwidth;
	beta = value + windowwidth;
	thevalue = negaScout(b,move,depth,alpha,beta,flags);
	if (((thevalue > alpha) && (thevalue < beta)) ||
	(flags->signal & FLAG_SIGNAL_OUTOFTIME)) return thevalue;	
	else {
		return negaScout(b,move,depth,MINVALUE - 1, MAXVALUE + 1, flags);
	}
}
