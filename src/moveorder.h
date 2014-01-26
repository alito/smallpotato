/*
   Copyright (C) 2003 Alejandro Dubrovsky
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
#ifndef MOVEORDER_H
#define MOVEORDER_H 1
#include "board.h"

#define MAXHISTORYSHIFT 19
#define MAXHISTORYVALUE (1 << MAXHISTORYSHIFT)


void orderMoveList(Board *board, Move *movelist, 
		int totalmoves, int *moveorder, Move *suggmove);
void updateKiller(Board *b, Move *m);
void clearKillers(Board *b, int depth);
//inline Move getKiller(Board *b, int depth, int which);
void makeBetaersSmaller(Board *b);


static inline Move getKiller(Board *b, int depth, int which) {
	return b->killers[depth][which].move;
}

#endif
