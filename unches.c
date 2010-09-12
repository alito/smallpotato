/* 
	smallpotato - multithreaded chess program

	Copyright (C) 2000 Alejandro Dubrovsky

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <getopt.h>
#include <libale.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "cmdline.h"
#include "board.h"
#include "communication.h"
#include "search.h"
#include "eval.h"
#include "hash.h"
#include "ai.h"
#include "pieces.h"
#include "book.h"

#define EXIT_FAILURE 1
#define RANDOM_ENGINE 1
#define ONESTEP_ENGINE 2
#define MULTISTEP_ENGINE 3
#define ITERATIVE_ALPHABETA_ENGINE 4
#define COMPLETENEGAMAX_ENGINE 5
#define MTDF_ENGINE 6
#define NEGASCOUT_ENGINE 7

#define POSITIONPIECE_EVAL 3
#define RELATIVEPOSITIONPIECE_EVAL 4


static struct {
	char *logfilename, *gamefilename, *valuefilename, *bookfilename, *computername;
	int engine, verbose, evaltype, record, usevaluefile, usebook, random, maxdepth,
	hashkilobytes, hashentries, resign, epdmindepth, drawvalue, timeaggression;
} configvalues = {"thelog", "thegames.pgn", "aleweights.out", 
	"book.opn", NULL, NEGASCOUT_ENGINE,0,POSITIONPIECE_EVAL,0,1,1,0,0,32768,0,DEFAULT_RESIGN,
	6, DEFAULT_DRAWVALUE, DEFAULT_TIME_AGGRESSION};

static void processConfigFile(char *fname)
{
	FILE *rcFile = NULL;
	char buf[1024];
	char *tmp, *tmp2;
	
	if (fname) rcFile = fopen(fname,"r");
	if (!rcFile) rcFile = fopen("sp.rc", "r");
	if (!rcFile) rcFile = fopen("sp.ini","r");
	if(!rcFile) return;

	while(fgets(buf, 1023, rcFile)) {
		char *x = buf;
		char *key, *value;
	
		if(buf[0] == '#') continue;
	
		key = nextToken(&x, "=\t\n\r");
		if(key == NULL) continue;
	
		value = nextToken(&x, "\n\n\r");
		if(value == NULL) continue;
	
		tmp = lowercase(key);
		key = strip(tmp);
		free(tmp);

		if(!strcmp(key, "hashkilobytes")) {
			configvalues.hashkilobytes = parseInteger(value);
		} else if(!strcmp(key, "hashentries")) {
			configvalues.hashentries = parseInteger(value);
		} else if(!strcmp(key, "resign")) {
			configvalues.resign = - ((int) (parseDouble(value) * 100));
		} else if(!strcmp(key, "verbose")) {
			configvalues.verbose = parseInteger(value);
		} else if(!strcmp(key, "logfile")) {
			configvalues.logfilename = strip(value);
		} else if(!strcmp(key, "record")) {
			configvalues.record = 1;
		} else if(!strcmp(key, "gamefile")) {
			configvalues.gamefilename = strip(value);
		} else if(!strcmp(key, "random")) {
			configvalues.random = parseInteger(value);
		} else if(!strcmp(key, "searcher")) {
			tmp = strip(value);
			tmp2 = lowercase(tmp);
			free(tmp);
			if (!strcmp(tmp2,"random")) {
				configvalues.engine = RANDOM_ENGINE;
			} else if (!strcmp(tmp2,"negascout")) {
				configvalues.engine = NEGASCOUT_ENGINE;
			} else if (!strcmp(tmp2,"onestep")) {
				configvalues.engine = ONESTEP_ENGINE;
			} else if (!strcmp(tmp2,"complete")) {
				configvalues.engine = COMPLETENEGAMAX_ENGINE;
			} else if (!strcmp(tmp2,"fixeddepth")) {
				configvalues.engine = MULTISTEP_ENGINE;
			}
			free(tmp2);
		} else if(!strcmp(key, "evaluation")) {
			tmp = strip(value);
			tmp2 = lowercase(tmp);
			free(tmp);
			if (!strcmp(tmp2,"standard")) {
				configvalues.evaltype = POSITIONPIECE_EVAL;
			} else if (!strcmp(tmp2,"relative")) {
				configvalues.evaltype = RELATIVEPOSITIONPIECE_EVAL;
			}
			free(tmp2);
		} else if(!strcmp(key, "computername")) {
			configvalues.computername = strip(value);
		} else if(!strcmp(key, "bookfile")) {
			configvalues.bookfilename = strip(value);
		} else if(!strcmp(key, "valuefile")) {
			configvalues.valuefilename = strip(value);
		} else if(!strcmp(key, "nobook")) {
			configvalues.usebook = 0;
		} else if(!strcmp(key, "novaluefile")) {
			configvalues.usevaluefile = 0;
		} else if(!strcmp(key, "noresign")) {
			configvalues.resign = MINVALUE - 1;
		} else if(!strcmp(key, "maxdepth")) {
			configvalues.maxdepth = parseInteger(value);
  		} else if(!strcmp(key, "epdmindepth")) {
			configvalues.epdmindepth = parseInteger(value);
		} else if(!strcmp(key, "drawvalue")) {
			configvalues.drawvalue = parseInteger(value);
		} else if(!strcmp(key, "timeaggression")) {
			configvalues.timeaggression = parseInteger(value);
		}

		free(key);
	}
	fclose(rcFile);
}



int main(int argc, char **argv)
{
	void *retval;
	int engine, evaltype, record, errorcode, usevaluefile = 1;
	Board *state;
	threadParameter *threadparameter;
	char *errorstring;
	char *tempcheck;
	FILE *logfile, *gamefile;
	pthread_t inputThread, outputThread;	/* thread that takes care of input/output to user/front end */

	prodcons *input, *output;	  /* communication buffers for iothread */

	Cmdline *cmd;

	logfile = gamefile = NULL;
#ifdef WIN32
	/* copied from Dann Corbit to use under windows environment */
	 /* No buffering, please... */
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);
	/* and I *really* mean it, too! */
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
	fflush(NULL);
#endif
	
	srandom((unsigned int) time(NULL));
	state = (Board *) xmalloc(sizeof(Board));
	threadparameter = (threadParameter *) xmalloc(sizeof(threadParameter));
	cmd = parseCmdline(argc, argv);
	
	if (cmd->show_helpP)
		usage();

	if (cmd->show_versionP) {
		printf("%s %s\n", argv[0], VERSION);
		exit(0);
	}

	if (cmd->configfileP) {
		processConfigFile(cmd->configfile);		
	} else {
		processConfigFile(NULL);		
	}

	if ((cmd->verboseP) && (cmd->verbose > 0)){
		configvalues.verbose = cmd->verbose;
	}
	if (configvalues.verbose > 0) {
		if (cmd->logfileP) {
			logfile = fopen(cmd->logfile, "w");
		} else if (configvalues.logfilename) {
			logfile = fopen(configvalues.logfilename, "w");
		} else {
			logfile = NULL;
			configvalues.verbose = 0;
		}
		state->logfile = logfile;
	} else {
		state->logfile = NULL;
	}
	state->verbosity = configvalues.verbose;

	record = 0;
	gamefile = NULL;
	if ((cmd->recordP) || (configvalues.record)) {
		if (!cmd->norecordP) {
			record = 1;
			if (cmd->gamefileP) {
				gamefile = fopen(cmd->gamefile, "a");
			} else if (configvalues.gamefilename) {
				gamefile = fopen(configvalues.gamefilename, "a");
			} else {
				gamefile = NULL;
				record = 0;
			}
		}
	}
	state->record = record;
	state->gamefile = gamefile;

	if (cmd->randomvarianceP) {
		state->random = cmd->randomvariance;
	} else {
		state->random = configvalues.random;
	}


	engine = configvalues.engine;

	if (cmd->randomP) {
		engine = RANDOM_ENGINE;
	}
	if (cmd->onestepP) {
		engine = ONESTEP_ENGINE;
	}

	if (cmd->completeP) {
		engine = COMPLETENEGAMAX_ENGINE;
	}

	if (cmd->multistepP) {
		engine = MULTISTEP_ENGINE;
	}


	if (cmd->negascoutP) {
		engine = NEGASCOUT_ENGINE;
	}


	evaltype = configvalues.evaltype;

	usevaluefile = configvalues.usevaluefile;
	if (cmd->fixedevalP) {
		usevaluefile = 0;
	}

	if (cmd->relativeevalP) {
		evaltype = RELATIVEPOSITIONPIECE_EVAL;
	} else if (cmd->positionevalP) {
		evaltype = POSITIONPIECE_EVAL;
	}

	switch (engine) {
	case RANDOM_ENGINE:
		state->engine = randomPlay;
		break;
	case ONESTEP_ENGINE:
		state->engine = oneStep;
		break;
	case COMPLETENEGAMAX_ENGINE:
		state->engine = multiNegamax;
		break;
	case MULTISTEP_ENGINE:
		state->engine = multiStep;
		break;
	case NEGASCOUT_ENGINE:
		state->engine = timedIterative;
		state->searcher = negaScoutDriver;
		break;
	default:
		state->engine = NULL;
	}

	if (cmd->resignP) {
		configvalues.resign = - ((int) (cmd->resign * 100));
	}

	switch (evaltype) {
	case POSITIONPIECE_EVAL:
		if (cmd->noresignP) {
			state->resign = MINVALUE - 1;
		} else {
			state->resign = configvalues.resign;
		}
		state->evalboard = positionheuri;
		break;
	case RELATIVEPOSITIONPIECE_EVAL:
		state->evalboard = relativepositionheuri;
		state->resign = MINVALUE - 1;
		break;
	default:
		state->evalboard = NULL;
	}

	if (!cmd->nobookP) {
		if (cmd->bookfileP) {
			errorcode = initialiseOpeningBook(cmd->bookfile);
			if (errorcode) {
				errorstring = bookErrorToString(errorcode);
				fprintf(stderr,"%s\n",errorstring);
				free(errorstring);
			}
		} else if ((configvalues.usebook) && (configvalues.bookfilename)) {
			errorcode = initialiseOpeningBook(configvalues.bookfilename);
			if (errorcode) {
				errorstring = bookErrorToString(errorcode);
				fprintf(stderr,"%s\n",errorstring);
				free(errorstring);
			}			
		}
	}

	if (usevaluefile) {
		if (cmd->valuefileP) {
			initialisePieceValues(cmd->valuefile);
			configvalues.valuefilename = cmd->valuefile;
		} else if (configvalues.valuefilename) {
			initialisePieceValues(configvalues.valuefilename);			
		} else {
			initialisePieceValues(NULL);
		}
	} else {
		initialisePieceValues(NULL);
	}
	
	
		
	if (cmd->computernameP) {
		state->name = xstrdup(cmd->computername);
	} else if (configvalues.computername) {
		state->name = xstrdup(configvalues.computername);
	} else {
		if (configvalues.valuefilename) {
			state->name =
				(char *) xmalloc(strlen(PACKAGE_STRING) + 
				strlen(configvalues.valuefilename) + 2);
			tempcheck = state->name;
			state->name = strcpy(state->name, PACKAGE_STRING);
			state->name = strcat(state->name, " ");
			state->name = strcat(state->name, configvalues.valuefilename);
			if (tempcheck != state->name) {
				printf("fucked\n");
			}
		} else {
			state->name = (char *) xstrdup (PACKAGE_STRING);
		}

	}

	if (cmd->dP) {
		configvalues.maxdepth = cmd->d;
	}
	state->depth = configvalues.maxdepth;

	if (cmd->epdmindepthP) {
		configvalues.epdmindepth = cmd->epdmindepth;
	}
	state->epdmindepth = configvalues.epdmindepth;

	if (cmd->drawvalueP) {
		configvalues.drawvalue = cmd->drawvalue;
	}
	state->drawvalue = configvalues.drawvalue;
	state->timeaggression = configvalues.timeaggression;

	if (cmd->hashsizeP) {
		state->hashtable = initialiseHashTable(cmd->hashsize);
	} else if (cmd->hashkilobytesP) {
		state->hashtable = initialiseHashTableBySize(cmd->hashkilobytes);
	} else if (configvalues.hashentries) {
		state->hashtable = initialiseHashTable(configvalues.hashentries);
	} else if (configvalues.hashkilobytes) {
		state->hashtable = initialiseHashTableBySize(configvalues.hashkilobytes);
	} else {
		state->hashtable = initialiseHashTable(0);
	}

	initialiseMoveLists();
	initialiseGenerateFunctions();

	input = (prodcons *) xmalloc(sizeof(prodcons));
	output = (prodcons *) xmalloc(sizeof(prodcons));

	initComms(input);
	initComms(output);
	state->input = input;
	state->output = output;
	threadparameter->input = input;
	threadparameter->output = output;
	threadparameter->name = state->name;

	pthread_create(&inputThread, NULL, doInput, (void *) threadparameter);
	pthread_create(&outputThread, NULL, doOutput, (void *) threadparameter);
	
	play(state);

	pthread_cancel(inputThread);
	pthread_join(outputThread, &retval);
	finaliseComms(input);
	finaliseComms(output);
	closeOpeningBook();
	free(state);
	free(input);
	free(output);
	if (configvalues.verbose) {
		fflush(logfile);
		fclose(logfile);
	}
	if (record) {
		fflush(gamefile);
		fclose(gamefile);
	}
	exit(0);
}
