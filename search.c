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
#include "search.h"
#include "eval.h"
#include "pieces.h"
#include "board.h"
#include "utils.h"
#include "book.h"
#include "hash.h"
#include "ai.h"
#include "moveorder.h"
#include <libale.h>
#include <math.h>

/*
 * Find a quiet spot to evaluate the position.  Play through all the captures till there are no captures left.  Return evaluation then
 * If safeOnly, the position is assumed to be quiet already, so only check if it's not illegal and hashtable for better evaluations
 */
int quiescentSearch(Board * b, int alpha, int beta, Flags * flags, int safeOnly)
{
	pieceType color;
	U64 key, position;
	hashChunk *hashchunk;
	hashTable *hashtable;
	int current, numbermoves, gottime = 1, emptyqueue,
		tempalpha, bestwhen = 0, moveindex = 0;
	int best = MINVALUE - 1;
	Move tempmove, sugmove = NOMOVE, bestmove = NOMOVE;
	int moveorder[MAXMOVESPERNODE]; 	
	Move movelist[MAXMOVESPERNODE];
	FILE *logfile;
	Action inaction;
	struct timeval temptime;

	b->totalquiescent++;
	hashtable = b->hashtable;
	logfile = b->logfile;

	key = b->history[b->ply];
	position = key % hashtable->size;
	if (b->blackturn) {
		hashchunk = &hashtable->black[position];
		color = BLACKP;		
	} else {
		hashchunk = &hashtable->white[position];
		color = WHITEP;		
	}

	/* see if position is stored in hashtable 
	Note that we are checking for a hash entry before checking the legality of the 
	position.  We assume that only legal position will get entered in the hashtable */
	if ((hashchunk->depth >= 0) && (hashchunk->key == key)) {
		int hiok, lowok;
		b->totalhash++;
		if (hashchunk->flags == HASH_ACCURATE) {
			hiok = lowok = 1;
		} else if (hashchunk->flags == HASH_LOWBOUND) {
			lowok = 1;
			hiok = 0;
		} else if (hashchunk->flags == HASH_UPBOUND) {
			hiok = 1;
			lowok = 0;
		}

		if ((hiok) && (lowok)) {
			flags->value = FLAG_NORMAL;
			if (hashchunk->lbound >= MAXVALUE - MAXPLYPERGAME) {
				return hashchunk->lbound + hashchunk->ply - b->ply;
			}
			if (hashchunk->lbound <= MINVALUE + MAXPLYPERGAME) {
				return hashchunk->lbound - hashchunk->ply + b->ply;
			}
			return hashchunk->lbound;
		}

		if ((hashchunk->lbound >= beta) && (lowok)) {
			flags->value = FLAG_LOWCUT;
			if (hashchunk->lbound >= MAXVALUE - MAXPLYPERGAME) {
				return hashchunk->lbound + hashchunk->ply - b->ply;
			}										
			return hashchunk->lbound;
		}
		if ((hashchunk->ubound <= alpha) && (hiok)) {
			flags->value = FLAG_HIGHCUT;
			if (hashchunk->ubound <= MINVALUE + MAXPLYPERGAME) {
				return hashchunk->ubound - hashchunk->ply + b->ply;
			}										
			return hashchunk->ubound;
		}
		/*
		if ((lowok) && (hashchunk->lbound > alpha)) {
			if (b->verbosity > 15) {
				fprintf(logfile,
						  "modified alpha by hash level %d\t value %d\thash key %llu\n",
						  hashchunk->depth, hashchunk->lbound, key);
			}

			alpha = hashchunk->lbound;
		}
		if ((hiok) && (hashchunk->ubound < beta)) {
			if (b->verbosity > 15) {
				fprintf(logfile,
						  "modified beta by hash level %d\t value %d\thash key %llu\n",
						  hashchunk->depth, hashchunk->ubound, key);
			}
			beta = hashchunk->ubound;
		}*/

		sugmove.from = hashchunk->from;
		sugmove.to = hashchunk->to;
		sugmove.piece = (pieceType) hashchunk->piece;
	}

	tempalpha = alpha;	
	numbermoves = generateTentativeQuiescentMoveList(b,movelist);

	/* check if current position is legal */
	if (numbermoves < 0) {
		b->totalquiescent--;
		return ILLEGAL_POSITION;
	}
	/* grab current, prepiece grabbing value as the lowend */
	current = (*(b->evalboard)) (b);
	if (color == BLACKP) {
		current = -current;
	}
		
	if (safeOnly ||(current >= beta) || (numbermoves == 0)) {
		return current;		
	}

	if (current > best) {
		best = current;
		if (best > tempalpha) {
			tempalpha = best;
		}
	}

	if (numbermoves > 1) {
		orderMoveList(b,movelist,numbermoves,moveorder,&sugmove);
	} else {
		moveorder[0] = 0;
	}

	while ((moveindex < numbermoves) && (best < beta) && (gottime)) {
		tempmove = movelist[moveorder[moveindex++]];
		movePiece(b, &tempmove);
		current = -quiescentSearch(b, -beta, -tempalpha, flags, 0);
		unMove(b);
		if (flags->signal & FLAG_SIGNAL_OUTOFTIME) {
			gottime = 0;
		} else {
			if (current > best) {
				bestwhen = moveindex;
				best = current;
				bestmove = tempmove;
				if (best > tempalpha) {
					tempalpha = best;
				}
			}
			
			if (b->totalnodes + b->totalquiescent > b->lastcheck + NODES_PER_CHECK) {
				b->lastcheck = b->totalnodes + b->totalquiescent;
				gettimeofday(&temptime,NULL);
				if ((timercmp(&temptime,&(flags->stoptime),>)) && (!(b->actionmode & ANALYSEMODE))) {
					flags->signal |= FLAG_SIGNAL_OUTOFTIME;
					gottime = 0;
				} else {
					emptyqueue = tryGetAction(b->input, &inaction);
					if (!emptyqueue) {
						handleActionWhileInSearch(b, &inaction, flags);
						if (flags->signal & FLAG_SIGNAL_OUTOFTIME) gottime = 0;
					}
				}
			}
		}
	}

	if (gottime) {
		if (best >= beta) {
			flags->value = FLAG_LOWCUT;
		} else {
			if (best <= alpha) {
				flags->value = FLAG_HIGHCUT;
			} else {
				flags->value = FLAG_NORMAL;
			}
		}
		/* store evaluation and best moves of current board into hashtable, but only
		if there was no entry there at all*/
		
		if ((hashchunk->depth < 0)
			/*|| (hashchunk->ply + hashchunk->depth < b->ply)*/) {  
			if (flags->value == FLAG_LOWCUT) {
				hashchunk->lbound = best;
				hashchunk->flags = HASH_LOWBOUND;
			}  else {
				if (flags->value == FLAG_HIGHCUT) {
					hashchunk->ubound = best;
					hashchunk->flags = HASH_UPBOUND;       
				} else {
					hashchunk->ubound = hashchunk->lbound = best;
					hashchunk->flags = HASH_ACCURATE;   
				}
			}
			  
			hashchunk->ply = (short int) b->ply;
			hashchunk->key = key;
			hashchunk->depth = 0;
			hashchunk->from = (unsigned char) bestmove.from;
			hashchunk->to = (unsigned char) bestmove.to;
			hashchunk->piece = (char) bestmove.piece;
		} 
		 
		if ((b->verbosity > 13)) {
			fprintf(logfile, "QUIESCENT - checked %d\n", bestwhen);
		}
	}
	return best;
}

/* Try to recover the PV from the transposition table, return is as a SAN string
ready for output */
static char * recoverPV(Board *b) {
	/* should probably recode at some point to be one pass, instead of two */
	
	U64 key, position;
	hashChunk *hashchunk;
	hashTable *hashtable;
	char *pvline;
	char buffer[10];
	int length = 0, stop = 0, pvlinelength = 1;

	hashtable = b->hashtable;
	
	do {	
		key = b->history[b->ply];
		position = key % hashtable->size;
	
		if (b->blackturn) {
			hashchunk = &hashtable->black[position];
		} else {
			hashchunk = &hashtable->white[position];
		}
		
		/* keep on going while there is a correct key and the position got stored during
		the main tree, or during quiescent search, but in this case, only if the best move
		is not to sit */
		if ((hashchunk->key == key) && ((hashchunk->depth > 0) || 
		((hashchunk->depth == 0) && ((hashchunk->from != 0) || (hashchunk->to != 0))))) {
			b->pv.line[length].from = hashchunk->from;
			b->pv.line[length].to = hashchunk->to;
			b->pv.line[length].piece = (pieceType) hashchunk->piece;
			if ((hashchunk->from == 0) && (hashchunk->to == 0)) 
				printf("here at length %d - depth %d\n",length, hashchunk->depth);
			moveToSan(b,&(b->pv.line[length]), buffer);
			pvlinelength += strlen(buffer) + 1;
			movePiece(b,&(b->pv.line[length]));
			if (isRepetitionDraw(b)) {
				stop = 1;
			}
			length++;
		} else stop = 1;
	} while ((!stop) && (length < MAXPLYPERGAME));
	b->pv.length = length;
	for (;length > 0; length--) unMove(b);	
	pvline = (char *) xmalloc(pvlinelength);
	*pvline = '\0';
	for (length = 0; length < b->pv.length; length++) {
		moveToSan(b,&(b->pv.line[length]), buffer);
		strcat(pvline,buffer);
		strcat(pvline," ");
		movePiece(b,&(b->pv.line[length]));
	}
	for (;length > 0; length--) unMove(b);	
	return pvline;
}

void calculateFinishTime(Board *b, struct timeval *timestarted, struct timeval *dontstartmark, Flags *flags) {
	struct timeval temptime, finishtime, hardlimit;
	timeControl tc;
	int hunspermove, hardtime = 0, hunstillstop;
	FILE *logfile;
	
	logfile = b->logfile;
	tc = b->timecontrol;
	if (b->timelimit > 0) {
		/* if we've been manually set a time limit per move, use it */
		hardtime = 1;
		hunspermove = b->timelimit;

	} else {
		int movesleft;
		if (tc.moves > 0) {
			/* if the time control is move based, calculate number of moves left till next
			time control */
			movesleft = tc.moves - ((b->ply + 1) >> 1);
			while (movesleft <= 0) {
				movesleft += tc.moves;
			}

		} else {
			/* if there is no per move time control, assume a game of 50 moves, but never
			leave less than 10 moves left (heuristic)
			FIXME:  make runtime modifiable or at least configurable */
			movesleft = 50 - ((b->ply + 1) >> 1);
			if (movesleft < 10) {
				movesleft = 10;
			}
		}
		hunspermove =
			(b->ourtime +
			 tc.increment * (movesleft - 1)) / movesleft;

	}
	temptime.tv_sec = (int) hunspermove / 100;
	temptime.tv_usec = (int) ((hunspermove % 100) * 10000);
	/* supposed finish time */
	timeradd(timestarted, &temptime, &finishtime);

	if (hardtime) {
		flags->stoptime = finishtime;
	} else {
		/* set the stop time inside to twice the time we've got per move (heuristic)*/ 
		timeradd(&finishtime, &temptime, &(flags->stoptime));
	}

	if (b->verbosity > 0) {
		fprintf(logfile, "Started at %ld.%06ld second\n",
				  (long int) timestarted->tv_sec, (long int) timestarted->tv_usec);
		fprintf(logfile,
				  "Supposedly finishing in less than %ld.%06ld seconds\n",
				  (long int) temptime.tv_sec, (long int) temptime.tv_usec);

	}
	if (b->timelimit > 0) {
		hunstillstop = hunspermove;
	} else {
		hunstillstop = (int) ((float) hunspermove * (b->timeaggression / 100.0));
	}
	temptime.tv_usec = (int) ((hunstillstop % 100) * 10000);
	temptime.tv_sec = (int) hunstillstop / 100;
	timeradd(timestarted, &temptime, dontstartmark);

	if (hardtime) {
		hardlimit = finishtime;
	} else {
		temptime.tv_usec = (int) ((b->ourtime % 100) * 10000);
		temptime.tv_sec = (int) b->ourtime / 100;
		timeradd(timestarted, &temptime, &hardlimit);
	}

	/* if this isn't an epd, reduce the hard limit by 1 second so we get time to send
	the move */
	if (b->epdmove.count <= 0) {
		temptime.tv_usec = 0;
		temptime.tv_sec = 1;
		timersub(&hardlimit, &temptime, &hardlimit);
	}

	if (timercmp(dontstartmark, &hardlimit, >)) {
		*dontstartmark = hardlimit;
	}

	if (timercmp(&(flags->stoptime),&hardlimit, >)) {
		/* if the heuristics lead us to go past the hardlimit, fix that */
		flags->stoptime = hardlimit;
	}

	if (b->verbosity > 0) {
		timersub(&hardlimit, timestarted, &temptime);
		fprintf(logfile,
				  "Hard limit in %ld.%06ld seconds\n",
				  (long int) temptime.tv_sec, (long int) temptime.tv_usec);

	}

}

int timedIterative(Board * b, Move * move)
{
	pieceType color;
	Move tempmove, currbest;
	Action action;
	Move movelist[MAXMOVESPERNODE];
	U64 key, position;

	hashChunk *hashchunk;
	hashTable *hashtable;
	FILE *logfile;
	struct timeval timestarted, dontstartmark, temptime, timesincestart;
	int best, current, currlevel, necessarymoves, levelcomplete, maxdepth,
		canmove, incheck, gottime, thebest, totalmoves, diffmove,
		startnodes, endnodes, moveindex, numbermoves, epdfound,
		startevals, endevals, starthash, endhash, startquiescent, endquiescent, checkmate,
		initialeval, i;

	char buffer[10];
	char *pvline;
	Flags flags;

	gettimeofday(&timestarted, NULL);

	if (b->blackturn) {
		color = BLACKP;
	} else {
		color = WHITEP;
	}

	hashtable = b->hashtable;
	hashchunk = NULL;
	logfile = b->logfile;
	/* check book */
	if ((!getBookMove(b,move)) && (!(b->actionmode & ANALYSEMODE))) {
		/*found book move*/
		return NORMAL;
	}

	/* check if we are in a phase change */
	if (checkPhase(b)) {
		setColorTotals(b);
		clearHashTable(b->hashtable);
	}

	calculateFinishTime(b, &timestarted, &dontstartmark, &flags);
	
	currlevel = gottime = 1;
	epdfound = levelcomplete = checkmate = 0;

	if (b->depth > 0) {
		maxdepth = b->depth;
	} else {
		maxdepth = MAXDEPTH;
	}

	/* get original position's eval for resign purposes.  We resign if both this
	original, and the calculated evals are under the resign values, with the added
	condition that the calculated has to be worse than the initial */
	initialeval = (*(b->evalboard)) (b);
	if (color == BLACKP) {
		initialeval = -initialeval;
	}
	
	
	best = MINVALUE - 1;
	totalmoves = necessarymoves = 0;
	thebest = best;
	while ((currlevel <= maxdepth) && (gottime) && (!checkmate) && (!epdfound)) {
		startnodes = b->totalnodes;
		startevals = b->totalevals;
		starthash = b->totalhash;
		startquiescent = b->totalquiescent;
		/* do a onestepper to start with to check for only moves, and instant checkmates */
		if (currlevel == 1) {
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
							current = b->drawvalue;
						}
					} else {
						if (b->fifty[b->ply] >= 100) {
							current = b->drawvalue;
						} else {
							if (isRepetitionDraw(b)) {
								current = b->drawvalue;
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
			gettimeofday(&temptime, NULL);
			necessarymoves = (int) ((double) numbermoves * 0.5);
			*move = currbest;
			tempmove = currbest;
		} else {
			flags.signal = FLAG_SIGNAL_NONE;
			b->nulldepth = 0;
			best = (*(b->searcher)) (b, &tempmove, currlevel, best, &flags);
			if ((flags.signal & FLAG_SIGNAL_OUTOFTIME) == 0) {
				currbest = tempmove;
			} else {
				gottime = 0;
			}
		}
		if (((gottime) || (levelcomplete == 0)) && (b->anomaly == NORMAL)) {
			*move = currbest;
			thebest = best;
			levelcomplete = currlevel;

			if (currlevel > 1) {
				key = b->history[b->ply];
				position = key % hashtable->size;
				if (b->blackturn) {
					hashchunk = &hashtable->black[position];
				} else {
					hashchunk = &hashtable->white[position];
				}
				hashchunk->lbound = hashchunk->ubound = best;
				hashchunk->key = key;
				hashchunk->depth = currlevel;
				hashchunk->flags = HASH_ACCURATE;
				hashchunk->from = currbest.from;
				hashchunk->to = currbest.to;
				hashchunk->piece = (char) currbest.piece;
						
				if ((best >= MAXVALUE - b->ply - currlevel + 2) || 
				(best <= MINVALUE + b->ply + currlevel - 2)) {
					checkmate = 1;
				}
			}

			currlevel++;
			if (currlevel > 2) {
				gettimeofday(&temptime, NULL);
				if ((timercmp(&temptime, &dontstartmark, >)) && (!(b->actionmode & ANALYSEMODE))) {
					gottime = 0;
				}
				
				if ((b->post) || (b->actionmode & ANALYSEMODE)) {
					endnodes = b->totalnodes;
					endevals = b->totalevals;
					endhash = b->totalhash;
					endquiescent = b->totalquiescent;
					pvline = recoverPV(b);
					/*moveToSan(b,move, buffer); */
					timersub(&temptime, &timestarted, &timesincestart);

					/* post thinking */
					action.theType = SP_POSTLINE;
					action.command = NULL;
					action.data.postline.ply = levelcomplete;
					action.data.postline.score = thebest;
					action.data.postline.nodes = endnodes - startnodes + endquiescent - startquiescent;
					action.data.postline.pv = xstrdup(pvline);
					action.data.postline.time = timesincestart.tv_sec * 100 + 
					timesincestart.tv_usec / 10000;
					putAction(b->output,&action);
					free(pvline);
				}

				/* check if we are doing an epd */
				if ((levelcomplete >= b->epdmindepth) && (b->epdmove.count > 0)) {					
					if (b->epdmove.avoid) {
						diffmove = TRUE;
						for (i = 0; (i < b->epdmove.count) && (diffmove); i++) {
							diffmove = (b->epdmove.moves[i].from != move->from) ||
							(b->epdmove.moves[i].to != move->to) ||
							(b->epdmove.moves[i].piece != move->piece);			
						}
						epdfound = diffmove;
					} else {
						epdfound = FALSE;
						for (i = 0; (i < b->epdmove.count) && (!epdfound); i++) {
							epdfound = (b->epdmove.moves[i].from == move->from) &&
							(b->epdmove.moves[i].to == move->to) &&
							(b->epdmove.moves[i].piece == move->piece);			
						}
					}
				}
			}
		}
		if (b->verbosity > 0) {
			endnodes = b->totalnodes;
			endevals = b->totalevals;
			endhash = b->totalhash;
			endquiescent = b->totalquiescent;
			moveToSan(b,move, buffer);
			timersub(&temptime, &timestarted, &timesincestart);
			fprintf(logfile, "at %ld.%06ld second\t",
					  (long int) timesincestart.tv_sec,
					  (long int) timesincestart.tv_usec);
			fprintf(logfile, "last level completed %d\t", levelcomplete);
			fprintf(logfile, "branching factor %5.2f\n",pow((double) (endnodes - startnodes),1.0 / ((double) levelcomplete)));
			fprintf(logfile, "level nodes %d\n", endnodes - startnodes + endquiescent - startquiescent);
			fprintf(logfile, "level evaluations %d\n", endevals - startevals);
			fprintf(logfile, "level hashes %d\n", endhash - starthash);		
			fprintf(logfile, "level quiescent nodes %d\n", endquiescent - startquiescent);			
			fprintf(logfile, "best score %d\t", thebest);
			fprintf(logfile, "move %s\n", buffer);
			/*if (b->verbosity > 5) {
				fprintf(logfile, "checked %d out of %d moves\n", bestwhen,
						  totalmoves);
			}*/

		}
	}
	if (b->anomaly == NORMAL) {
		if (b->actionmode & ANALYSEMODE) {
			if ((currlevel > maxdepth) || (checkmate)) {
				b->anomaly = WAIT_FOR_EVENT;
			}
		} else if ((initialeval < b->resign) && (thebest < initialeval) && 
		(levelcomplete >= 4)) {
			b->anomaly = MOVE_AND_RESIGN;	
		}
	}
	
	return NORMAL;
}

