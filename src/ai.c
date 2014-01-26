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
#include <libale.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ai.h"
#include "search.h"
#include "eval.h"
#include "utils.h"
#include "hash.h"
#include "bench.h"
#include "book.h"

#define BENCH_MOVEUNMOVE 10000000

int play(Board * state)
{
	int notquit, nothingthere, counter, result, playingblack, ourturn,
		finished, clearcommand, timeticking, moves,
		firstmove, ourrating, theirrating, defaultrandom,i,j,k, promptpos,
		movewidth, icsmode, totalmoves, errorcode, whattodo;
	Action inaction, action, todo;
	prodcons *input, *output;
	FILE *logfile, *gamefile;
	char *opponentname;
	char buffer[100];
	Move *move;
	double timelapse;
	struct timeval starttime, endtime, timediff;

	whattodo = NORMAL;
	icsmode = 0;
	result = 0;
	notquit = nothingthere = 1;
	counter = ourrating = theirrating = 0;
	playingblack = firstmove = 1;
	ourturn = 0;
	finished = 0;
	state->actionmode = NORMALMODE;
	timeticking = 0;
	promptpos = 0;
	opponentname = NULL;
	move = (Move *) xmalloc(sizeof(Move));
	input = state->input;
	output = state->output;
	logfile = state->logfile;
	gamefile = state->gamefile;
	defaultrandom = state->random;
	state->ponder = 0;
	state->timecontrol.moves = 0;
	state->timecontrol.base = 0;
	state->timecontrol.increment = 0;
	state->timelimit = 0;
	state->post = 0;
	state->epdmove.count = 0;
	
	if (state->record > 0) {
		fprintf(gamefile,"\n\n");
	}

	initialiseBoard(state);
	while (notquit) {
		nothingthere = tryGetAction(input, &inaction);
		if (nothingthere) {
			if ((!(state->actionmode & (FORCEMODE | BOOKMODE))) || (state->actionmode & ANALYSEMODE)) {
				if ((ourturn) || (state->actionmode & ANALYSEMODE)) {
					if (finished == 0) {
						state->totalnodes = state->totalevals = state->totalhash = 
						state->totalquiescent = state->lastcheck = 0;
						if (gettimeofday(&starttime, NULL) < 0) {
							fprintf(stderr, "can't get time\n");
							fflush(stderr);
						}
						
						/* clear the history table */
						for (i = 0; i < 2; i++)
							for (j = 0; j < 128; j++)
								for (k = 0; k < 128; k++)
									state->betaers[i][j][k] = 0;

						state->actionmode &= ~ (PONDERMODE);
						whattodo = (*(state->engine)) (state, move, state);
						if (state->anomaly & (NORMAL | MOVE_AND_ACT | MOVE_AND_RESIGN)) {
							gettimeofday(&endtime, NULL);
							movewidth = moveToSan(state,move, buffer);
							if (state->verbosity > 0) {
								fprintf(logfile, "move %d.", (state->ply + 2) >> 1);
								if (state->blackturn) {
									fprintf(logfile, "  ");
								}
								fprintf(logfile, "%s\n", buffer);
								timersub(&endtime, &starttime, &timediff);
								timelapse =
									timediff.tv_sec + (timediff.tv_usec / 1000000.0);
								fprintf(logfile, "total time %ld secs %ld usecs\n",
										  (long int) timediff.tv_sec,
										  (long int) timediff.tv_usec);
	
								fprintf(logfile, "total nodes %d\n", state->totalnodes + state->totalquiescent);
								fprintf(logfile,
										  "%f nodes per sec\n", (state->totalnodes + state->totalquiescent) / timelapse);
								fprintf(logfile, "total evaluations %d\n", state->totalevals);
								fprintf(logfile,
										  "%f evaluations per sec\n", state->totalevals / timelapse);							
								fprintf(logfile, "total hashes %d\n", state->totalhash);			
								fprintf(logfile,
										  "%f hashes per sec\n", state->totalhash / timelapse);
								fprintf(logfile, "total quiescent nodes %d\n", state->totalquiescent);			
								fprintf(logfile,
										  "%f quiescent nodes per sec\n\n", state->totalquiescent / timelapse);
												
	
	
							}
							if (state->record > 0) {
								if (firstmove) {
									firstmove = 0;
									printPGNHeader(gamefile, state->name,
														 opponentname, ourrating,
														 theirrating,
														 &state->timecontrol,
														 playingblack);
									promptpos = 0;
								}
								if (promptpos + movewidth + 4 > 79) {
									promptpos = 0;
									fprintf(gamefile,"\n");
								}
								if (state->blackturn) {
									fprintf(gamefile, " %s ", buffer);
									promptpos += movewidth + 2;
								} else {
									fprintf(gamefile, "%d. %s",
											  (state->ply + 2) >> 1, buffer);
									promptpos += movewidth + 4;										  
								}
							}
							movePiece(state, move);
							ourturn = 0;
							action.theType = MOVE;
							action.data.move = *move;
							putAction(output, &action);
							if (isCheckmate(state)) {
								finished = 1;
								action.theType = RESULT;	
								if (state->blackturn) {
									action.data.result.won = WHITEWON;
									action.data.result.comment =
										(char *) xstrdup("White mates");
								} else {
									action.data.result.won = BLACKWON;
									action.data.result.comment =
										(char *) xstrdup("Black mates");
								}
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);
							} else if (isStalemate(state)) {
								finished = 1;
								action.theType = RESULT;
								action.data.result.won = RESDRAW;
								action.data.result.comment =
									(char *) xstrdup("Stalemate");
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);
							} else if (state->fifty[state->ply] >= 100) {
								finished = 1;
								action.theType = RESULT;
								action.data.result.won = RESDRAW;
								action.data.result.comment =
									(char *) xstrdup("50 Move rule");
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);
							} else if (isRepetitionDraw(state)) {
								finished = 1;
								action.theType = RESULT;
								action.data.result.won = RESDRAW;
								action.data.result.comment =
									(char *) xstrdup("Repetition draw");
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);
							} else if (isInsufficientMaterialDraw(state)) {
								finished = 1;
								action.theType = RESULT;
								action.data.result.won = RESDRAW;
								action.data.result.comment =
									(char *) xstrdup("Insufficient material");
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);								
							} else if (state->anomaly & MOVE_AND_RESIGN) {
								finished = 1;
								action.theType = RESULT;	
								if (!state->blackturn) {
									action.data.result.won = WHITEWON;
									action.data.result.comment =
										(char *) xstrdup("Black resigns");
								} else {
									action.data.result.won = BLACKWON;
									action.data.result.comment =
										(char *) xstrdup("White resigns");
								}
								if (state->record) {
									printResult(gamefile, &action.data.result);
								}
								putAction(output, &action);
							}
						} else if (state->anomaly & WAIT_FOR_EVENT) {
							getAction(input, &inaction);
							nothingthere = 0;							
						}
					}

				} else {
					if (state->ponder) {
						state->actionmode |= PONDERMODE;
						counter++;
					} else {
						getAction(input, &inaction);
						nothingthere = 0;
					}
				}
			}
		}
		if ((!nothingthere) || (state->anomaly & (ACT | MOVE_AND_ACT))) {
			clearcommand = 1;
			action.command = NULL;
			if (!nothingthere) todo = inaction;
			else {
				todo = state->lastaction;
			}
			state->anomaly = NORMAL;
			switch (todo.theType) {
			case USERMOVE:
				if ((todo.data.move.from >= 0)
					 && ((ourturn == 0) || (state->actionmode & (FORCEMODE | BOOKMODE | ANALYSEMODE)))
					 && (isLegal(state, &todo.data.move))) {
					 	
					if (state->actionmode & BOOKMODE) {
						addBookMove(state,&todo.data.move);
					} else {
						movewidth = moveToSan(state, &todo.data.move, buffer);
						if (state->verbosity > 0) {
							fprintf(logfile, "move %d.", (state->ply + 2) >> 1);
							if (state->blackturn) {
								fprintf(logfile, "  ");
							}
							fprintf(logfile, "%s\n", buffer);
						}
						if (state->record) {
							if (firstmove) {
								firstmove = 0;
								printPGNHeader(gamefile, state->name,
													 opponentname, ourrating,
													 theirrating,
													 &state->timecontrol, playingblack);
								promptpos = 0;
							}
							if (promptpos + movewidth + 4 > 79) {
								promptpos = 0;
								fprintf(gamefile,"\n");
							}
	
							if (state->blackturn) {
								fprintf(gamefile, " %s ", buffer);
								promptpos += movewidth + 2;
							} else {
								fprintf(gamefile, "%d. %s",
										  (state->ply + 2) >> 1, buffer);
								promptpos += movewidth + 4;									  
							}
						}
					}
					movePiece(state, &todo.data.move);								
					if (isCheckmate(state)) {
						finished = 1;
						action.theType = RESULT;
						if (state->blackturn) {
							action.data.result.won = WHITEWON;
							action.data.result.comment =
								(char *) xstrdup("White mates");
						} else {
							action.data.result.won = BLACKWON;
							action.data.result.comment =
								(char *) xstrdup("Black mates");
						}
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					} else if (isStalemate(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Stalemate");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}	
						putAction(output, &action);
					} else if (state->fifty[state->ply] >= 100) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("50 Move rule");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);	
					} else if (isRepetitionDraw(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Repetition draw");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					} else if (isInsufficientMaterialDraw(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Insufficient material");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					}
					if (!(state->actionmode & (BOOKMODE | FORCEMODE))) {
						ourturn = 1;
						timeticking = 1;
					}
				} else {
					action.theType = ERROR;
					action.data.message = (char *) xstrdup("Illegal Move");
					action.command = todo.command;
					clearcommand = 0;
					putAction(output, &action);
				}
				break;

			case SP_SANMOVE:
				if ((!sanToMove(state,todo.data.message,move)) 
				 && ((ourturn == 0) || (state->actionmode & (FORCEMODE | BOOKMODE | ANALYSEMODE)))
				 && (isLegal(state, move))) {
					if (state->actionmode & BOOKMODE) {
						addBookMove(state,move);
					} else {
						movewidth = moveToSan(state, move, buffer);
						if (state->verbosity > 0) {
							fprintf(logfile, "move %d.", (state->ply + 2) >> 1);
							if (state->blackturn) {
								fprintf(logfile, "  ");
							}
							fprintf(logfile, "%s\n", buffer);
						}
						if (state->record) {
							if (firstmove) {
								firstmove = 0;
								printPGNHeader(gamefile, state->name,
													 opponentname, ourrating,
													 theirrating,
													 &state->timecontrol, playingblack);
								promptpos = 0;
							}
							if (promptpos + movewidth + 4 > 79) {
								promptpos = 0;
								fprintf(gamefile,"\n");
							}
	
							if (state->blackturn) {
								fprintf(gamefile, " %s ", buffer);
								promptpos += movewidth + 2;
							} else {
								fprintf(gamefile, "%d. %s",
										  (state->ply + 2) >> 1, buffer);
								promptpos += movewidth + 4;									  
							}
						}
					}
					movePiece(state, move);								
					if (isCheckmate(state)) {
						finished = 1;
						action.theType = RESULT;
						if (state->blackturn) {
							action.data.result.won = WHITEWON;
							action.data.result.comment =
								(char *) xstrdup("White mates");
						} else {
							action.data.result.won = BLACKWON;
							action.data.result.comment =
								(char *) xstrdup("Black mates");
						}
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					} else if (isStalemate(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Stalemate");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}	
						putAction(output, &action);
					} else if (state->fifty[state->ply] >= 100) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("50 Move rule");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);	
					} else if (isRepetitionDraw(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Repetition draw");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					} else if (isInsufficientMaterialDraw(state)) {
						finished = 1;
						action.theType = RESULT;
						action.data.result.won = RESDRAW;
						action.data.result.comment =
							(char *) xstrdup("Insufficient material");
						if (state->record) {
							printResult(gamefile, &action.data.result);
						}
						putAction(output, &action);
					}
					if (!(state->actionmode & (BOOKMODE | FORCEMODE))) {
						ourturn = 1;
						timeticking = 1;
					}


				} else {
					action.theType = USERERROR;
					action.command = todo.command;
					clearcommand = 0;
					action.data.message = xstrdup(todo.command);
					putAction(output, &action);						
				}
				free(todo.data.message);					
				break;


			case NEW:
				initialiseBoard(state);
				finished = 0;
				playingblack = firstmove = 1;
				ourturn = ourrating = theirrating = 0;
				timeticking = 0;
				state->actionmode &= ~(FORCEMODE);
				state->timelimit = 0;
				state->timecontrol.moves = 0;
				state->timecontrol.base = 0;
				state->timecontrol.increment = 0;
				state->epdmove.count = 0;
				state->depth = 0;
				state->random = 0;
				if (icsmode) {
					action.theType = TELLICS;
					action.data.message = (char *) xstrdup("seek 1 0 r");
					putAction(output, &action);
					action.data.message = (char *) xstrdup("seek 3 0 r");
					putAction(output, &action);
					action.data.message = (char *) xstrdup("seek 5 0 r");
					putAction(output, &action);
				}
				break;
			case FORCE:
				state->actionmode |= FORCEMODE;
				timeticking = 0;
				break;
			case TIME:
				state->ourtime = todo.data.time;
				break;
			case QUIT:
				action.theType = QUIT;
				putAction(output, &action);
				notquit = 0;
				break;
			case RANDOM:

				if (state->random) {
					state->random = 0;
				} else {
					state->random = defaultrandom;
				}
				setPieceValues(state);
				setColorTotals(state);
				break;
			case GO:
				state->actionmode = NORMALMODE;
				playingblack = state->blackturn;
				ourturn = 1;
				timeticking = 1;
				break;
			case PLAYOTHER:
				state->actionmode = NORMALMODE;
				timeticking = 1;
				if (state->blackturn) {
					playingblack = 0;
				} else {
					playingblack = 1;
				}
				ourturn = 0;
				break;
			case LEVEL:
				state->timecontrol = todo.data.timecontrol;
				break;
			case SD:
				state->depth = todo.data.depth;
				break;
			case ST:
				state->timelimit = todo.data.time;
				break;
			case OTIM:
				state->theirtime = todo.data.time;
				break;
			case MOVENOW:
				break;
			case PING:
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case XBOARD:
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case PROTOVER:
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case ACCEPTED:
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case REJECTED:
				putAction(output, &todo);
				clearcommand = 0;
				break;

			case DRAW:
				break;
			case RESULT:
				if ((state->record) && (finished == 0)) {
					printResult(gamefile, &todo.data.result);
				}
				if (todo.data.result.comment != NULL) {
					free(todo.data.result.comment);
				}
				finished = 1;
				break;
			case SETBOARD:
				moves = parseFEN(todo.data.message, state);
				if (moves < 0) {
					action.theType = USERERROR;
					action.data.message = (char *) xstrdup("Illegal position");
				} else {
					if (state->verbosity > 0) {
						fprintf(logfile,"Setboard: %s\n\n", todo.data.message);
						printBoard(logfile, state);
						
					}
					if (state->record > 0) {
						fprintf(gamefile,"Setboard: %s\n\n", todo.data.message);						
						printBoard(gamefile, state);
					}
				}
				free(todo.data.message);
				break;
			case EDIT:
				break;
			case HINT:
				break;
			case BK:
				action.theType = ERROR;
				action.data.message = (char *) xstrdup("Not supported yet");
				action.command = todo.command;
				clearcommand = 0;
				putAction(output, &action);

				break;
			case UNDO:
				if (state->ply > 0) {
					unMove(state);
				}
				break;
			case REMOVE:
				if (state->ply > 1) {
					unMove(state);
					unMove(state);
				}

				break;
			case HARD:
				/*state->ponder = 1; */
				break;
			case EASY:
				state->ponder = 0;
				break;
			case POST:
				state->post = 1;				
				break;
			case NOPOST:
				state->post = 0;							
				break;
			case ANALYZE:
				state->actionmode |= ANALYSEMODE;
				break;
			case EXIT:
				state->actionmode = NORMALMODE;
				break;
			case UPDATESTATUS:
				break;
			case NAME:
				if (todo.data.message != NULL) {
					if (opponentname != NULL) {
						free(opponentname);
					}
					opponentname = (char *) xstrdup(todo.data.message);
					free(todo.data.message);
				}
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case RATING:
				ourrating = todo.data.ratings.ourrating;
				theirrating = todo.data.ratings.theirrating;
				break;
			case ICS:
				icsmode = 1;
				putAction(output, &todo);
				clearcommand = 0;
				break;
			case COMPUTER:
				putAction(output, &todo);
				clearcommand = 0;
				break;

			case DEBUG:
				free(todo.data.message);
				fprintf(stderr, "Ourturn: %d\nPlayingblack %d\n", ourturn,
						  playingblack);
				fprintf(stderr, "Mode: %d\nFinished%d\n", state->actionmode,
						  finished);
				fprintf(stderr, "Timeticking: %d\nPondering %d\n", timeticking,
						  state->ponder);
				fprintf(stderr, ": %d\nPlayingblack %d\n", ourturn, playingblack);
				fprintf(stderr, "Random factor %d\tMaxdepth %d\n",state->random,state->depth);
				fprintf(stderr, "Phase: %d\n",state->phase);
				fprintf(stderr, "White total: %d\tBlack total: %d\n",state->whitetotal,
				state->blacktotal);
				fflush(stderr);
				break;
			case SP_MOVETOSAN:
				action.theType = USERMESSAGE;
				movewidth = moveToSan(state, &todo.data.move, buffer);								
				movePiece(state, &todo.data.move);
				action.data.message = (char *) xstrdup(buffer);	
				putAction(output, &action);
				break;

			case SP_BENCHMOVEUNMOVE:
				action.theType = USERMESSAGE;
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "starting move/unmove bench");
				putAction(output, &action);
				
				gettimeofday(&starttime, NULL);				
				benchMoveUnmove(state, &todo.data.move, BENCH_MOVEUNMOVE);	
				gettimeofday(&endtime, NULL);
				timersub(&endtime, &starttime, &timediff);
				timelapse =
					timediff.tv_sec + (timediff.tv_usec / 1000000.0);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "%d move/unmove cycles\t%f move/unmoves per sec\n", BENCH_MOVEUNMOVE, BENCH_MOVEUNMOVE / timelapse);
				putAction(output, &action);
				break;


			case SP_BENCHMOVEGEN:
				action.theType = USERMESSAGE;				
				i = todo.data.time;
				gettimeofday(&starttime, NULL);								
				totalmoves = benchMoveGen(state, i);
				gettimeofday(&endtime, NULL);				
				timersub(&endtime, &starttime, &timediff);
				timelapse =
					timediff.tv_sec + (timediff.tv_usec / 1000000.0);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "%f move generation cycles per sec\n", i / timelapse);
				putAction(output, &action);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "%f moves generated per sec\n", totalmoves / timelapse);
				putAction(output, &action);

				break;

			case SP_BENCHEVAL:
				action.theType = USERMESSAGE;				
				i = todo.data.time;
				gettimeofday(&starttime, NULL);								
				benchEval(state, i);
				gettimeofday(&endtime, NULL);				
				timersub(&endtime, &starttime, &timediff);
				timelapse =
					timediff.tv_sec + (timediff.tv_usec / 1000000.0);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "%f evals per sec\n", i / timelapse);
				putAction(output, &action);
				break;


			case SP_BENCHMOVECYCLE:
				action.theType = USERMESSAGE;				
				i = todo.data.time;
				gettimeofday(&starttime, NULL);								
				totalmoves = benchMoveCycle(state, i);
				gettimeofday(&endtime, NULL);				
				timersub(&endtime, &starttime, &timediff);
				timelapse =
					timediff.tv_sec + (timediff.tv_usec / 1000000.0);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "total moves=%d\ttime=%f\tgenmove/move/unmove %f/sec", totalmoves, timelapse, totalmoves/timelapse);
				putAction(output, &action);

				break;


			case SP_PERFT:
				action.theType = USERMESSAGE;				
				i = todo.data.depth;
				gettimeofday(&starttime, NULL);								
				totalmoves = benchPerft(state, i);
				gettimeofday(&endtime, NULL);				
				timersub(&endtime, &starttime, &timediff);
				timelapse =
					timediff.tv_sec + (timediff.tv_usec / 1000000.0);
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "total moves=%d\ttime=%f\tgenmove/move/unmove %f/sec", totalmoves, timelapse, totalmoves/timelapse);
				putAction(output, &action);

				break;

			case SP_EPDSUITE:
				benchEPDTestSuite(state, todo.data.message);
				free(todo.data.message);			
				break;

			case SP_EPDLINE:
				benchEPDLine(state, todo.data.message);
				free(todo.data.message);			
				break;
			
			case SP_EPDMINDEPTH:
				state->epdmindepth = todo.data.depth;
				break;

			case SP_MODIFYBOOK:
				errorcode = modifyOpeningBook(todo.data.message);
				if (errorcode) {
					action.theType = USERERROR;				
					action.command = NULL;
					action.data.message = bookErrorToString(errorcode);
					putAction(output, &action);
				}
				free(todo.data.message);
				state->actionmode = BOOKMODE;
				break;

			case SP_CLOSEBOOK:
				closeOpeningBook();
				state->actionmode = NORMALMODE;
				break;
				
			case SP_SHOWMOVES:
				displayLegalMoves(state);
				break;

			case SP_EVALUATE:
				if (checkPhase(state)) setColorTotals(state);
				i = (*(state->evalboard)) (state);
				action.theType = USERMESSAGE;
				action.data.message = (char *) xmalloc(100);				
				sprintf(action.data.message,
									  "evaluation %d", i);
				putAction(output, &action);
				
				break;

			case WHITE:
				break;
			case BLACK:
				break;
			case VARIANT:
				break;

			case HELP:
				putAction(output, &todo);
				clearcommand = 0;
				break;

			case UNKNOWN_COMMAND:
				action.theType = USERERROR;
				action.data.message = xstrdup("Didn't understand that last bit");
				putAction(output, &action);
				break;
			default:;

			}

			
			if (clearcommand == 1) {
				if (todo.command != NULL) {
					free(todo.command);
				}
			}
		}
	}
	free(move);
	return result;
}

void handleActionWhileInSearch(Board *state, Action *action, Flags *flags) {
	int clearcommand = 1;
	Action newaction;
	switch (action->theType) {
		case MOVENOW:
			flags->signal |= FLAG_SIGNAL_OUTOFTIME;		
			break;
		case HARD:
			/*state->ponder = 1; */
			break;
		case EASY:
			state->ponder = 0;
			break;
		case POST:
			state->post = 1;				
			break;
		case NOPOST:
			state->post = 0;							
			break;
		case ANALYZE:
			state->actionmode |= ANALYSEMODE;
			break;
		case DRAW:
			break;			
		case HINT:
			break;
		case UPDATESTATUS:
			break;
		case PING:
			putAction(state->output, action);
			clearcommand = 0;
			break;
		case HELP:
			putAction(state->output, action);
			clearcommand = 0;
			break;

		case UNKNOWN_COMMAND:
			newaction.theType = USERERROR;
			newaction.data.message = xstrdup("Didn't understand that last bit");
			putAction(state->output, &newaction);
			break;
			
		default:
			clearcommand = 0;
			flags->signal |= FLAG_SIGNAL_OUTOFTIME;
			state->anomaly = ACT;
			state->lastaction = *action;
	}	
	if (clearcommand == 1) {
		if (action->command != NULL) {
			free(action->command);
		}
	}
	
}
