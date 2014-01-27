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
#include "bench.h"
#include "search.h"
#include "utils.h"
#include "hash.h"
#include <libale.h>


void benchMoveUnmove(Board *board, Move *move, int howmany) {
	int i;
	for (i = 0; i < howmany; i++) {
		movePiece(board, move);
		unMove(board);
	}
	
}

int benchMoveGen(Board *board, int howmany) {
	Move movelist[500];
	int numbermoves = 0, i;
	for (i = 0; i < howmany; i++) {	
		numbermoves = generateMoveList(board,movelist);		
	}
	return numbermoves * howmany;
}

int benchPerft(Board * b, int depth) {
	Move movelist[MAXMOVESPERNODE];
	Move tempmove;
	int totalmoves, numbermoves, moveindex;

	numbermoves = generateMoveList(b,movelist);
	totalmoves = 0;
	if (depth > 1) {
		for (moveindex = 0; moveindex < numbermoves; moveindex++) {
			tempmove = movelist[moveindex];			
			movePiece(b, &tempmove);
			totalmoves += benchPerft(b, depth - 1);
			unMove(b);		
		}
	} else {
		totalmoves = numbermoves;
	}
	return totalmoves;
}

int benchMoveCycle(Board * b, int depth) {
	Move movelist[MAXMOVESPERNODE];
	int totalmoves, numbermoves, moveindex;

	numbermoves = generateMoveList(b,movelist);
	totalmoves = 0;
	if (depth > 1) {
		for (moveindex = 0; moveindex < numbermoves; moveindex++) {
			movePiece(b, &(movelist[moveindex]));
			totalmoves += benchMoveCycle(b, depth - 1);
			unMove(b);		
		}
	} else {
		for (moveindex = 0; moveindex < numbermoves; moveindex++) {
			movePiece(b, &(movelist[moveindex]));
			unMove(b);		
		}
		totalmoves = numbermoves;
	}
	return totalmoves;
}

void benchEval(Board *b, int iterations) {
	int i;
	for (i = 0; i < iterations; i++) {
		(*(b->evalboard)) (b);
	}
}

void benchEPDTestSuite(Board *b,char *filename) {
	FILE *epdfile;
	char *line, *semicolon, *idline, *beginquote, *endquote;
	int total = 0, solved = 0;
	Action show;
	
	if (filename == NULL) return;
	epdfile = fopen(filename,"r");	
	if (epdfile == NULL) return;
	
	show.command = NULL;
	show.theType = USERMESSAGE;
	while (!feof(epdfile)) {
		line = fsafegets(epdfile, 300);
		if (line && strlen(line)) {
			semicolon = strchr(line,';');
			if (semicolon) {
				beginquote = strchr(semicolon,'"');
				*semicolon = '\0';
				if (beginquote) {
					idline = beginquote + 1;
					endquote = strrchr(idline,'"');
					if (endquote) {
						*endquote = '\0';
						show.data.message = xstrdup(idline);
						putAction(b->output,&show);
					}
				}
			}
			printf("%s\n",line);
			total++;
			if (benchEPDLine(b, line)) solved++;
		}		
		free(line);
	}
	
	fclose(epdfile);	
	show.data.message = (char *) xmalloc(200);
	sprintf(show.data.message,"Solved %d out of %d in under %d seconds",solved, total,
	b->timelimit/100);
	putAction(b->output,&show);
}

int benchEPDLine(Board *board,char *epdline) {
	char *stripped;
	int prevtimelimit, diffmove, epdfound = TRUE, i;
	Move move;
	Action show;	
	struct timeval timestarted, timefinished, difftime;
	
	if ((epdline == NULL) || (strlen(epdline) == 0)) return FALSE;
	stripped = strip(epdline);

	/* Now parse the EPD line */
	initialiseBoard(board);
	if (!parseEPD(stripped, board)) {
		printf("epd parsing failed\n");	
		free(stripped);
		board->epdmove.count = 0;
		return FALSE;
	}
	gettimeofday(&timestarted, NULL);
	prevtimelimit = board->timelimit;
	if (board->timelimit == 0) 
		board->timelimit = 1000000000;  /* just to set the time limit to something large */
	(*(board->engine)) (board, &move);	
	gettimeofday(&timefinished, NULL);
	timersub(&timefinished, &timestarted, &difftime);

	if (board->epdmove.avoid) {
		diffmove = TRUE;
		for (i = 0; (i < board->epdmove.count) && (diffmove); i++) {
			diffmove = (board->epdmove.moves[i].from != move.from) ||
			(board->epdmove.moves[i].to != move.to) ||
			(board->epdmove.moves[i].piece != move.piece);			
		}
		epdfound = diffmove;
	} else {
		epdfound = FALSE;
		for (i = 0; (i < board->epdmove.count) && (!epdfound); i++) {
			epdfound = (board->epdmove.moves[i].from == move.from) &&
			(board->epdmove.moves[i].to == move.to) &&
			(board->epdmove.moves[i].piece == move.piece);			
		}
	}
	
	
	show.command = NULL;
	show.theType = USERMESSAGE;
	show.data.message = (char *) xmalloc(100);				
	if (epdfound)
		sprintf(show.data.message,
						  "solved in %ld.%06ld seconds",
						  (long int) difftime.tv_sec,
					 	 (long int) difftime.tv_usec);
	else
		sprintf(show.data.message, 
						  "not solved in %ld.%06ld seconds",
						  (long int) difftime.tv_sec,
					 	 (long int) difftime.tv_usec);
		
	putAction(board->output,&show);
	board->timelimit = prevtimelimit;
	free(stripped);
	return epdfound;
}

void displayLegalMoves(Board *b) {
	Move movelist[MAXMOVESPERNODE];
	Action out;
	int i, numbermoves;

	numbermoves = generateMoveList(b,movelist);
	out.command = NULL;
	out.theType = USERMESSAGE;
	out.data.message = (char *) xmalloc(100);	
	sprintf(out.data.message,
						  "%d moves\n", numbermoves);
	
	putAction(b->output, &out);	
	out.data.message = NULL;
	out.theType = MOVE;	
	for (i = 0; i < numbermoves; i++) {
		out.data.move = movelist[i];
		putAction(b->output, &out);	
	}	
}

