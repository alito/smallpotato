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
#ifndef EVAL_H
#define EVAL_H 1
#include "board.h"

#define PAWNVALUE 100
#define ROOKVALUE 500
#define KNIGHTVALUE 300
#define BISHOPVALUE 300
#define QUEENVALUE 900
#define KINGVALUE 10000000
#define MAXVALUE KINGVALUE
#define MINVALUE -KINGVALUE
#define DEFAULT_DRAWVALUE 0
#define DEFAULT_RESIGN 6.5

int initialisePieceValues(char *finname);
void setPieceValues(Board *state);
int checkPhase(Board *board);
int positionheuri(Board * board);
int relativepositionheuri(Board * board);


#endif
