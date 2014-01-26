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

#ifndef UTILS_H
#define UTILS_H 1

#include "common.h"
#include "board.h"

featureType stringToFeature(char *feature);
char *featureToString(featureType feature);
int parseLevel(char *levelString, timeControl * timecontrol);
int parseResult(char *astring, Result * result);
int parseRatings(char *astring, Ratings * ratings);
int parseMove(char *astring, Move * amove);
int parseFEN(char *fen, Board * board);
int parseEPD(char *epd, Board * board);
void moveToString(Move * themove, char *buffer);
int moveToSan(Board *board, Move * themove, char *buffer);
int sanToMove(Board *board, char *san, Move *move);
void printResult(FILE * out, Result * result);
void printGameHeader(FILE * out, char *ourname, char *opponentname,
							int ourrating, int theirrating,
							timeControl * timecontrol, int playingblack);
void printPGNHeader(FILE * out, char *ourname, char *opponentname,
					 int ourrating, int theirrating,
					 timeControl * timecontrol, int playingblack);

void printBoard(FILE * out, Board * board);


#endif
