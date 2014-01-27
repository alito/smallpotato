/*
   Copyright (C) 2002 Alejandro Dubrovsky
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

#ifndef BOOK_H
#define BOOK_H 1
#include "piecesstruct.h"
#include "board.h"

int modifyOpeningBook(char *filename);

/* Open book stored in filename for reading. Return 0 on success, non-zero on error */
int initialiseOpeningBook(char *filename);

/* Open book in filename for writing. If it doesn't exist, create it. Returns 0 on success, non-zero on error */
int getBookMove(Board *board, Move *move);

/* Close opening book */
void closeOpeningBook();

/* Convert error code to a newly-malloced string */
char *bookErrorToString(int error);

/* Add move to currently open book at position board */
int addBookMove(const Board *board, const Move *move);

#endif
