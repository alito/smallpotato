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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H 1

#include "common.h"
#include <pthread.h>
#include <semaphore.h>
#include "piecesstruct.h"

#define BUFFER_SIZE 8

enum featureEnumType
{ FPING, FSETBOARD, FPLAYOTHER, SAN, FUSERMOVE, FTIME, FDRAW, FSIGINT, FSIGTERM,
		REUSE,
	FANALYZE, MYNAME, VARIANTS, COLORS, FICS, FNAME, DONE, UNKNOWN
};

typedef enum featureEnumType featureType;

enum actionEnumType
{ XBOARD, PROTOVER, ACCEPTED, REJECTED, NEW, FORCE, TIME, QUIT, PRINT,
		VARIANT, RANDOM, GO, PLAYOTHER,
	WHITE, BLACK, LEVEL, SD, ST,
	OTIM, USERMOVE, MOVENOW, PING, DRAW, RESULT, SETBOARD, EDIT, HINT, BK,
		UNDO, REMOVE, HARD, EASY, POST, UPDATESTATUS,
	NOPOST, ANALYZE, NAME, RATING, ICS, COMPUTER, MOVE, ERROR, USERERROR, EXIT,
		DEBUG, USERMESSAGE, TELLICS, SP_MOVETOSAN, SP_BENCHMOVEUNMOVE, 
		SP_BENCHMOVEGEN, SP_PERFT, SP_BENCHMOVECYCLE, SP_BENCHEVAL, SP_EPDSUITE,
		SP_EPDLINE, SP_POSTLINE, SP_MODIFYBOOK, SP_CLOSEBOOK, SP_SANMOVE, SP_SHOWMOVES,
		SP_EVALUATE, HELP, UNKNOWN_COMMAND, SP_EPDMINDEPTH
};

typedef enum actionEnumType actionType;


enum resultEnumType
{ WHITEWON, BLACKWON, RESDRAW, UNDECIDED };

typedef enum resultEnumType resultType;



struct ratingsStruct
{
	int ourrating, theirrating;
};

typedef struct ratingsStruct Ratings;

struct timeControlStruct
{
	int moves, base, increment;
};

typedef struct timeControlStruct timeControl;


struct resultTypeStruct
{
	resultType won;
	char *comment;
};

typedef struct resultTypeStruct Result;

struct postTypeStruct
{
	int ply;   /* ply depth */
	int score;	/* score in centipawns  (not in smallpotato's case) */
	int time;	/* score in centiseconds */
	int nodes;	/* nodes searched */
	char *pv;	/* pv line */
};

typedef struct postTypeStruct Post;

union actionDataType
{
	Move move;
	timeControl timecontrol;
	Result result;
	Ratings ratings;
	int depth;
	int time;
	char *message;
	featureType feature;
	Post postline;
};

typedef union actionDataType actionData;

struct actionStruct
{
	actionType theType;
	actionData data;
	char *command;
};

typedef struct actionStruct Action;


struct prodconsStruct
{
	Action buffer[BUFFER_SIZE];  /* the actual data */
	int readpos, writepos;		  /* positions for reading and writing */
	sem_t sem_read;				  /* number of elements available for reading */
	sem_t sem_write;				  /* number of locations available for writing */
};

typedef struct prodconsStruct prodcons;

struct threadParamStruct
{
	prodcons *input, *output;
	char *name;
};

typedef struct threadParamStruct threadParameter;


void putAction(prodcons * b, Action * data);
void getAction(prodcons * b, Action * data);
int tryPutAction(prodcons * b, Action * data);
int tryGetAction(prodcons * b, Action * data);
void *doInput(void *details);
void *doOutput(void *details);
void initComms(prodcons * b);
void finaliseComms(prodcons * b);
/*int isThereSomethingToRead(prodcons *b);*/

#endif
