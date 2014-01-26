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

#ifndef BENCH_H
#define BENCH_H 1

#include "common.h"
#include "piecesstruct.h"


void benchMoveUnmove(Board *board, Move *move, int howmany);
int benchMoveGen(Board *board, int howmany);
int benchPerft(Board * board, int depth);
void benchEval(Board *b, int iterations);
int benchMoveCycle(Board * b, int depth);
void benchEPDTestSuite(Board *b, char *filename);
int benchEPDLine(Board *b,char *epdline);
void displayLegalMoves(Board *b);

#endif
