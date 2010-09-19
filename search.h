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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef SEARCH_H
#define SEARCH_H 1
#include "common.h"
#include "board.h"
#include <sys/time.h>

#define MAXMOVESPERNODE 500
#define MAXDEPTH 110
#define DEFAULT_TIME_AGGRESSION 35

#define BETACUT 1
#define BELOWALPHA 2
#define NOCUT 3

#define HASH_UPBOUND 1
#define HASH_ACCURATE 2
#define HASH_LOWBOUND 4
#define FLAG_LOWCUT 1
#define FLAG_HIGHCUT 2
#define FLAG_NORMAL 0
#define FLAG_SIGNAL_NONE 0
#define FLAG_SIGNAL_USEMOVE 1
#define FLAG_SIGNAL_OUTOFTIME 2
#define FLAG_SIGNAL_DONTHASH 4
#define ILLEGAL_POSITION 250000000
#define NODES_PER_CHECK 60000

#define DEPTH_REDUCTION 2

struct flagsStruct
{
	int draw;
	int checkmate;
	int value;
	int signal;
	struct timeval stoptime;
};

typedef struct flagsStruct Flags;
/*
struct moveChainStruct
{
	Move move;
	struct moveChainStruct *nextmove;
};

typedef struct moveChainStruct MoveChain;
*/


int randomPlay(Board * b, Move * move);
int oneStep(Board * b, Move * move);
int multiStep(Board * b, Move * move);
int multiNegamax(Board * b, Move * move);
int timedIterative(Board * b, Move * move);
int quiescentSearch(Board *b, int alpha, int beta, Flags *flags, int safeOnly);
void calculateFinishTime(Board *b, struct timeval *timestarted, struct timeval *dontstartmark, Flags *flags);
int negaScoutDriver(Board * b, Move * move, int depth, int value, Flags *flags);



#endif
