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
#include <stdio.h>
#include <libale.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "utils.h"

void *doInput(void *details)
{
	prodcons *toProg;
	Action action;
	const size_t BUFF_INC = 1000;
	char inputBuffer[BUFF_INC], *wholeBuffer, *newline, *stripped, *theinput,
		*firstspace, *command;
	char *rest;
	int temp, xboardmode = 0, done = 0;
	size_t commandLength, currentMaxLength, currentBufferLength;


#ifdef DEBUG	
	FILE *debugFile;
	debugFile = fopen("iolog.log","w");
	setlinebuf(debugFile);
#endif
	newline = NULL;
	toProg = ((threadParameter *) details)->input;
	wholeBuffer = (char *) xmalloc(BUFF_INC);
	currentMaxLength = BUFF_INC;
	while (done == 0) {
		currentBufferLength = 0;
		memset((void *) wholeBuffer, '\0', currentMaxLength);
		do {
			if (fgets(inputBuffer, BUFF_INC, stdin) == NULL) {
				done = -1;
				break;
			}
			currentBufferLength += BUFF_INC;
			if (currentBufferLength > currentMaxLength) {
				wholeBuffer = (char *) xrealloc((void *) wholeBuffer, currentBufferLength);
				currentMaxLength = currentBufferLength;
			}
			wholeBuffer = strcat(wholeBuffer, inputBuffer);
			newline = strchr(wholeBuffer, '\n');
		} while (newline == NULL);

		if (newline == NULL) {
			fprintf(stderr, "Error reading from input\n");
			continue;
		}
		*newline = '\0';
#ifdef DEBUG		
		fprintf(debugFile,"%s\n",wholeBuffer);
#endif
		stripped = strip(wholeBuffer);
		action.command = xstrdup(stripped);
		theinput = uppercase(stripped);
		free(stripped);
		if ((theinput != NULL) && (strlen(theinput) > 0)) {
			firstspace = strchr(theinput, ' ');
			if (firstspace == NULL) {
				commandLength = strlen(theinput);
				rest = NULL;
			} else {
				commandLength = firstspace - theinput;
				rest = xstrdup(firstspace + 1);
			}
			/* if (rest != NULL) {
			 * printf("rest: %s\n",rest);
			 * } */
			command = (char *) xmalloc(commandLength + 1);
			memcpy(command, theinput, commandLength);
			command[commandLength] = '\0';

			if (strcmp(command, "USERMOVE") == 0) {
				action.theType = USERMOVE;
				if (parseMove(rest, &action.data.move) < 0) {
					action.data.move.from = -1;
				}
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_SANMOVE") == 0) || 
			((!xboardmode) && (strcmp(command, "SANMOVE") == 0))) {
				action.theType = SP_SANMOVE;
				action.data.message = (char *) xstrdup(action.command + commandLength + 1);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if (strcmp(command, "XBOARD") == 0) {
				xboardmode = 1;
				action.theType = XBOARD;
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "PROTOVER") == 0) {
				action.theType = PROTOVER;
				action.data.message = rest;
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "ACCEPTED") == 0) {
				action.theType = ACCEPTED;
				action.data.feature = stringToFeature(rest);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}

				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "REJECTED") == 0) {
				action.theType = REJECTED;
				action.data.feature = stringToFeature(rest);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}

				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "NEW") == 0) {
				action.theType = NEW;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}

				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "VARIANT") == 0) {
				action.theType = VARIANT;
				action.data.message = rest;
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "QUIT") == 0) {
				action.theType = QUIT;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}

				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "RANDOM") == 0) {
				action.theType = RANDOM;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "FORCE") == 0) {
				action.theType = FORCE;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}

				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "GO") == 0) {
				action.theType = GO;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "PLAYOTHER") == 0) {
				action.theType = PLAYOTHER;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "WHITE") == 0) {
				action.theType = WHITE;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "BLACK") == 0) {
				action.theType = BLACK;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "LEVEL") == 0) {
				action.theType = LEVEL;
				if (parseLevel(rest, &action.data.timecontrol) == 0) {
					putAction(toProg, &action);
				}
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "SD") == 0) {
				action.theType = SD;
				action.data.depth = parseInteger(rest);
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "ST") == 0) {
				action.theType = ST;
				action.data.time = parseInteger(rest) * 100;
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "TIME") == 0) {
				action.theType = TIME;
				action.data.time = parseInteger(rest);
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "OTIM") == 0) {
				action.theType = OTIM;
				action.data.time = parseInteger(rest);
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "?") == 0) {
				action.theType = MOVENOW;
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "PING") == 0) {
				action.theType = PING;
				action.data.time = parseInteger(rest);
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "DRAW") == 0) {
				action.theType = DRAW;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "RESULT") == 0) {
				action.theType = RESULT;
				if (parseResult(rest, &action.data.result) == 0) {
					putAction(toProg, &action);
				}
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "SETBOARD") == 0) {
				action.theType = SETBOARD;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				action.data.message =
					(char *) xstrdup(action.command + commandLength + 1);
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "EDIT") == 0) {
				action.theType = EDIT;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "HINT") == 0) {
				action.theType = HINT;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "BK") == 0) {
				action.theType = BK;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "UNDO") == 0) {
				action.theType = UNDO;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "REMOVE") == 0) {
				action.theType = REMOVE;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "HARD") == 0) {
				action.theType = HARD;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "EASY") == 0) {
				action.theType = EASY;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "POST") == 0) {
				action.theType = POST;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "NOPOST") == 0) {
				action.theType = NOPOST;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "EXIT") == 0) {
				action.theType = EXIT;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if (strcmp(command, "ANALYZE") == 0) {
				action.theType = ANALYZE;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if (strcmp(command, ".") == 0) {
				action.theType = UPDATESTATUS;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if (strcmp(command, "NAME") == 0) {
				action.theType = NAME;
				action.data.message = rest;
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "RATING") == 0) {
				action.theType = RATING;
				temp = parseRatings(rest, &action.data.ratings);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				if (temp == 0) {
					putAction(toProg, &action);
				} else {
					free(action.command);
				}
				goto ENDPARSE;
			}
			if (strcmp(command, "ICS") == 0) {
				action.theType = ICS;
				action.data.message = rest;
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if (strcmp(command, "COMPUTER") == 0) {
				action.theType = COMPUTER;
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}
			if ((strcmp(command, "SP_DEBUG") == 0) || 
			((!xboardmode) && (strcmp(command, "DEBUG") == 0))){
				action.theType = DEBUG;
				action.data.message = rest;
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			if ((strcmp(command, "SP_MOVETOSAN") == 0) || 
			((!xboardmode) && (strcmp(command, "MOVETOSAN") == 0))){
				action.theType = SP_MOVETOSAN;
				if (parseMove(rest, &action.data.move) < 0) {
					action.data.move.from = -1;
				}
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_BENCHMOVEUNMOVE") == 0) || 
			((!xboardmode) && (strcmp(command, "BENCHMOVEUNMOVE") == 0)))  {
				action.theType = SP_BENCHMOVEUNMOVE;
				if (parseMove(rest, &action.data.move) < 0) {
					action.data.move.from = -1;
				}
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_BENCHMOVEGEN") == 0) || 
			((!xboardmode) && (strcmp(command, "BENCHMOVEGEN") == 0))){
				action.theType = SP_BENCHMOVEGEN;
				action.data.time = parseInteger(rest);				
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_BENCHMOVECYCLE") == 0) || 
			((!xboardmode) && (strcmp(command, "BENCHMOVECYCLE") == 0))) {
				action.theType = SP_BENCHMOVECYCLE;
				action.data.time = parseInteger(rest);				
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_BENCHEVAL") == 0) || 
			((!xboardmode) && (strcmp(command, "BENCHEVAL") == 0))) {
				action.theType = SP_BENCHEVAL;
				action.data.time = parseInteger(rest);				
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}



			if ((strcmp(command, "SP_PERFT") == 0) || 
			((!xboardmode) && (strcmp(command, "PERFT") == 0))) {
				action.theType = SP_PERFT;
				action.data.depth = parseInteger(rest);				
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_EPDSUITE") == 0) || 
			((!xboardmode) && (strcmp(command, "EPDSUITE") == 0))) {
				action.theType = SP_EPDSUITE;
				action.data.message = (char *) xstrdup(action.command  + commandLength + 1);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}				
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_EPDLINE") == 0) || 
			((!xboardmode) && (strcmp(command, "EPDLINE") == 0))) {
				action.theType = SP_EPDLINE;
				action.data.message = (char *) xstrdup(action.command  + commandLength + 1);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}				
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_MODIFYBOOK") == 0) || 
			((!xboardmode) && (strcmp(command, "MODIFYBOOK") == 0))) {
				action.theType = SP_MODIFYBOOK;
				action.data.message = (char *) xstrdup(action.command  + commandLength + 1);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}				
				putAction(toProg, &action);
				goto ENDPARSE;
			}
			
			if ((strcmp(command, "SP_CLOSEBOOK") == 0) || 
			((!xboardmode) && (strcmp(command, "CLOSEBOOK") == 0))) {
				action.theType = SP_CLOSEBOOK;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_SHOWMOVES") == 0) || 
			((!xboardmode) && (strcmp(command, "SHOWMOVES") == 0))) {
				action.theType = SP_SHOWMOVES;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_EVALUATE") == 0) || 
			((!xboardmode) && (strcmp(command, "EVALUATE") == 0))) {
				action.theType = SP_EVALUATE;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if (strcmp(command, "HELP") == 0) {
				action.theType = HELP;
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				putAction(toProg, &action);
				goto ENDPARSE;
			}

			if ((strcmp(command, "SP_EPDMINDEPTH") == 0) || 
			((!xboardmode) && (strcmp(command, "EPDMINDEPTH") == 0))) {
				action.theType = SP_EPDMINDEPTH;
				action.data.depth = parseInteger(rest);				
				putAction(toProg, &action);
				if (rest != NULL) {
					free(rest);
					rest = NULL;
				}
				goto ENDPARSE;
			}


			action.theType = UNKNOWN_COMMAND;
			if (rest != NULL) {
				free(rest);
				rest = NULL;
			}
			putAction(toProg, &action);
			

		 ENDPARSE:;
		}
		free(theinput);
		theinput = NULL;
	}
	free(wholeBuffer);
#ifdef DEBUG		
	fclose(debugFile);  
#endif
	return NULL;
}



void *doOutput(void *details)
{
	prodcons *fromProg;
	char buffer[100];
	char *ourname;
	Action currAction;
	threadParameter * parameters;
	int haventQuit, xboardmode, protover, icsmode;
	parameters = (threadParameter *) details;
	fromProg = parameters->output;
	ourname = parameters->name;
	haventQuit = 1;
	xboardmode = 0;
	icsmode = 0;
	protover = 1;
	memset(buffer, 0, 100);
	fprintf(stdout,"This is " PACKAGE_STRING ".  This is the last prompt you'll see\n"
	"If when you type a command, you get no error message, it means everything is ok\n"
	"'help' offers some hints > ");
	fflush(stdout);
	while (haventQuit) {
		getAction(fromProg, &currAction);
		switch (currAction.theType) {
		case QUIT:
			haventQuit = 0;
			break;
		case PRINT:
			if (currAction.data.message != NULL) {
				fprintf(stdout, "%s", currAction.data.message);
				fflush(stdout);
				free(currAction.data.message);
				currAction.data.message = NULL;
			}
			break;
		case ERROR:
			fprintf(stdout, "Error ");
			if (currAction.data.message != NULL) {
				fprintf(stdout, "(%s)", currAction.data.message);
			}
			fprintf(stdout, ": ");
			if (currAction.command != NULL) {
				fprintf(stdout, "%s", currAction.command);
			}
			fprintf(stdout, "\n");
			fflush(stdout);
			break;
		case USERERROR:
			if (xboardmode) {
				fprintf(stdout, "tellusererror ");
				if (currAction.data.message != NULL) {
					fprintf(stdout, "(%s)", currAction.data.message);
				}
				fprintf(stdout, "\n");
				fflush(stdout);
			} else {
				if (currAction.data.message != NULL) {
					fprintf(stderr, "Error: %s\n", currAction.data.message);
				}
			}
			break;
		case USERMESSAGE:
			if (xboardmode) {
				if (icsmode) {
					fprintf(stdout, "tellopponent %s\n", currAction.data.message);
				} else {
					fprintf(stdout, "telluser %s\n", currAction.data.message);
				}
			} else {
				fprintf(stdout, "%s\n", currAction.data.message);
			}
			fflush(stdout);
			if (currAction.data.message != NULL) {
				free(currAction.data.message);
			}
			break;
		case TELLICS:
			if ((xboardmode) && (icsmode)) {
				if (currAction.data.message != NULL) {
					fprintf(stdout, "tellics %s\n", currAction.data.message);
					fflush(stdout);
				}

			}
			if (currAction.data.message != NULL) {
				free(currAction.data.message);
				currAction.data.message = NULL;
			}

			break;
		case XBOARD:
			xboardmode = 1;
			fprintf(stdout, "\n");
			fflush(stdout);
			break;
		case PROTOVER:
			if (xboardmode == 1) {
				protover = atoi(currAction.data.message);
				if (protover == 2) {
					fprintf(stdout,
							  "feature ping=1 setboard=1 playother=1 usermove=1 sigint=0 sigterm=0 myname=\"%s\" variants=\"normal\" colors=0 ics=1 name=1 done=1\n",
							  ourname);
					fflush(stdout);
				}
			}
			if (currAction.data.message != NULL) {
				free(currAction.data.message);
				currAction.data.message = NULL;
			}
			break;
		case MOVE:
			if (xboardmode == 1) {
				moveToString(&currAction.data.move, buffer);
				fprintf(stdout, "move %s\n", buffer);
				fflush(stdout);
			} else {
				moveToString(&currAction.data.move, buffer);
				printf("%s\n", buffer);
			}
			break;
		case ACCEPTED:
			break;
		case REJECTED:
			break;
		case NEW:
			break;
		case TIME:
			break;
		case VARIANT:
			break;
		case PLAYOTHER:
			break;
		case WHITE:
			break;
		case BLACK:
			break;
		case PING:
			fprintf(stdout, "pong %d\n", currAction.data.time);
			fflush(stdout);
			break;
		case SP_POSTLINE:
			if (xboardmode) {
				fprintf(stdout, "%d %d %d %d %s\n", currAction.data.postline.ply, 
				currAction.data.postline.score, currAction.data.postline.time, 
				currAction.data.postline.nodes, 
				(currAction.data.postline.pv ? currAction.data.postline.pv:""));
			} else {
				fprintf(stdout, "Depth: %d ply\tScore: %d\t%f secs\t%d nodes\t%s\n", currAction.data.postline.ply, 
				currAction.data.postline.score, currAction.data.postline.time / 100.0, 
				currAction.data.postline.nodes, 
				(currAction.data.postline.pv ? currAction.data.postline.pv:""));
			}
			fflush(stdout);
			if (currAction.data.postline.pv != NULL)
				free(currAction.data.postline.pv);
			break;
		case RESULT:
			switch (currAction.data.result.won) {
			case WHITEWON:
				strcpy(buffer, "1-0");
				break;
			case BLACKWON:
				strcpy(buffer, "0-1");
				break;
			case RESDRAW:
				strcpy(buffer, "1/2-1/2");
				break;
			default:
				strcpy(buffer, "*");
				
			}
			fprintf(stdout, "%s", buffer);
			if (currAction.data.result.comment != NULL) {
				fprintf(stdout, " {%s}", currAction.data.result.comment);
				free(currAction.data.result.comment);
			}
			fprintf(stdout, "\n");
			fflush(stdout);
			break;
		case NAME:
			break;
		case RATING:
			break;
		case ICS:
			if (strcmp(currAction.data.message, "-") != 0) {
				icsmode = 1;
			}
			if (currAction.data.message != NULL) {
				free(currAction.data.message);
			}
			break;
		case COMPUTER:
			break;
		
		case UNKNOWN_COMMAND:
			fprintf (stdout,"that last bit i didn't understand \n");
			fflush(stdout);
			break;
		
		case HELP:
			fprintf(stdout,"You want help too?  So does everyone else\n"
			"The potato will take most IETF protocol version 2 commands which can be\n"
			"located at http://www.tim-mann.org/xboard/engine-intf.html\n"
			"Other commands can be found in the commands.txt file that should have\n"
			"come with this program.  Brief summary:\n"
			"(* denotes add sp_ before the commands if you are in xboard mode)\n"
			"(if you don't know what xboard mode is, then ignore the line above)\n"
			"To make a move type 'usermove <themove>' where <themove> is in e2e4 format or\n"
			"'sanmove <themove>' * where <themove> is in Nf3 format\n"
			"To set time controls, try 'st <seconds per move>' or\n"
			"'level 0 <minutes/game> <second increment/move>' (but this second\n"
			"option won't work very easily since you have to update how much time is\n"
			"left before each move with 'time <time>' in hundredths of a second)\n"
			"To reset game, type new\n"
			"To set board to some FEN, 'setboard <FEN>'\n"
			"To analyze a position 'analyze'\n"
			"To solve an epd, 'epdline <epd>' * (use st before to set a time limit for it)\n"
			"To go through an epd test suite, 'epdsuite <epdfile>'  *\n"
			"To quit, 'quit'\n");
			fflush(stdout);
			break;
		
		default:
			
			;
		}

		if (currAction.command != NULL) {
			free(currAction.command);
			currAction.command = NULL;
		}


	}

	return NULL;
}
