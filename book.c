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
 
#include "common.h"
#include "book.h"
#include <libale.h>
#include <gdbm.h>

#define BOOK_VERSION 1
#define BOOK_OK 0
#define BOOK_FAILED 1
#define BOOK_FAILCREATE 2
#define BOOK_FAILREQUIREDINSERT 3
#define BOOK_FAILOPEN 4
#define BOOK_WRONGVERSION 5
#define BOOK_NOTFOUND 6

#define KEYWKCASTLE 0
#define KEYWQCASTLE 1
#define KEYBKCASTLE 2
#define KEYBQCASTLE 3
#define KEYBLACKTURN 4

#define PALPBOOKHASHKEY 13

static GDBM_FILE openingBook = NULL;
static U64 bookkeys[14][64];
static U64 specialkeys[5];
static U64 getBookHashKey(Board * board);

struct bookMoveStructType {
	Move move;
	int ranking;
};

typedef struct bookMoveStructType BookMove; 

int modifyOpeningBook(char *filename) {
	int i,j, bookversion;
	datum key, data;
	
	if ((!filename) || (!strlen(filename))) return BOOK_FAILED;
	if (initialiseOpeningBook(filename)) {
		closeOpeningBook();
		openingBook = gdbm_open(filename,512, GDBM_NEWDB,0666,NULL);
		if (!openingBook) return BOOK_FAILCREATE;
		for (i = 0; i < 14; i++) {
			for (j = 0; j < 64; j++) {
				bookkeys[i][j] = (((U64) ((unsigned short int) random())) << 48) + 
									(((U64) ((unsigned short int) random())) << 32) +
									(((U64) ((unsigned short int) random())) << 16) +
									(((U64) ((unsigned short int) random())));
			}
		}
		specialkeys[KEYWQCASTLE] = (((U64) ((unsigned short int) random())) << 48) + 
										(((U64) ((unsigned short int) random())) << 32) +
										(((U64) ((unsigned short int) random())) << 16) +
										(((U64) ((unsigned short int) random())));
		specialkeys[KEYWKCASTLE] = (((U64) ((unsigned short int) random())) << 48) + 
										(((U64) ((unsigned short int) random())) << 32) +
										(((U64) ((unsigned short int) random())) << 16) +
										(((U64) ((unsigned short int) random())));
		specialkeys[KEYBQCASTLE] = (((U64) ((unsigned short int) random())) << 48) + 
										(((U64) ((unsigned short int) random())) << 32) +
										(((U64) ((unsigned short int) random())) << 16) +
										(((U64) ((unsigned short int) random())));
		specialkeys[KEYBKCASTLE] = (((U64) ((unsigned short int) random())) << 48) + 
										(((U64) ((unsigned short int) random())) << 32) +
										(((U64) ((unsigned short int) random())) << 16) +
										(((U64) ((unsigned short int) random())));
		specialkeys[KEYBLACKTURN] = (((U64) ((unsigned short int) random())) << 48) + 
										(((U64) ((unsigned short int) random())) << 32) +
										(((U64) ((unsigned short int) random())) << 16) +
										(((U64) ((unsigned short int) random())));

	
		key.dsize = strlen("BOOK_VERSION") + 1;
		key.dptr = "BOOK_VERSION";
		bookversion = BOOK_VERSION;
		data.dsize = sizeof(bookversion);
		data.dptr = (char *) &bookversion;
		if (gdbm_store(openingBook,key,data,GDBM_INSERT)) {
			closeOpeningBook();
			return BOOK_FAILREQUIREDINSERT;
		}
		key.dsize = strlen("HASHKEYS") + 1;
		key.dptr = "HASHKEYS";
		data.dsize = sizeof(bookkeys);
		data.dptr = (char *) bookkeys;
		if (gdbm_store(openingBook,key,data,GDBM_INSERT)) {
			closeOpeningBook();
			return BOOK_FAILREQUIREDINSERT;
		}
		key.dsize = strlen("SPECIALKEYS") + 1;
		key.dptr = "SPECIALKEYS";
		data.dsize = sizeof(specialkeys);
		data.dptr = (char *) specialkeys;
		if (gdbm_store(openingBook,key,data,GDBM_INSERT)) {
			closeOpeningBook();
			return BOOK_FAILREQUIREDINSERT;
		}	
	} else {
		closeOpeningBook();
		openingBook = gdbm_open(filename,512, GDBM_WRITER,0666,NULL);
		if (!openingBook) return BOOK_FAILOPEN;		
	}
	return BOOK_OK;
}

int initialiseOpeningBook(char *filename) {
	datum key, data;
	
	if ((!filename) || (!strlen(filename))) return BOOK_FAILED;
	closeOpeningBook();
	openingBook = gdbm_open(filename,512, GDBM_READER,0,NULL);
	if (!openingBook) return BOOK_FAILOPEN;
	key.dsize = strlen("BOOK_VERSION") + 1;
	key.dptr = "BOOK_VERSION";
	data = gdbm_fetch(openingBook,key);
	if ((data.dptr == NULL) || (*((int *)data.dptr) != BOOK_VERSION)) {
		closeOpeningBook();
		return BOOK_WRONGVERSION;
	}
	key.dsize = strlen("HASHKEYS") + 1;
	key.dptr = "HASHKEYS";
	data = gdbm_fetch(openingBook,key);
	if (data.dptr == NULL) {
		closeOpeningBook();
		return BOOK_FAILED;
	}
	memcpy(bookkeys,data.dptr,sizeof(bookkeys));	
	free(data.dptr);

	key.dsize = strlen("SPECIALKEYS") + 1;
	key.dptr = "SPECIALKEYS";
	data = gdbm_fetch(openingBook,key);
	if (data.dptr == NULL) {
		closeOpeningBook();
		return BOOK_FAILED;
	}
	memcpy(specialkeys,data.dptr,sizeof(specialkeys));
	free(data.dptr);
	return BOOK_OK;
}

void closeOpeningBook() {
	if (openingBook == NULL) return;
	gdbm_close(openingBook);
	openingBook = NULL;		
}

int getBookMove(Board *board, Move *move) {
	datum data, key;
	U64 hashkey;
	int howmany, ranking, i, totalranking;
	BookMove *allmoves;
	
	if (openingBook == NULL) return BOOK_FAILED;
	hashkey = getBookHashKey(board);
	key.dsize = sizeof(hashkey);
	key.dptr = (char *) &hashkey;
	data = gdbm_fetch(openingBook,key);
	if (data.dptr == NULL) return BOOK_NOTFOUND;
	howmany = data.dsize / sizeof(BookMove);
	if (howmany == 1) {
		*move = ((BookMove*) (data.dptr))->move;
	} else {
		allmoves = (BookMove *) data.dptr;
		totalranking = 0;
		for (i = 0; i < howmany; i++) totalranking += allmoves[i].ranking;
		if (totalranking == 0) {
			return BOOK_FAILED;
		}

		ranking = random() % totalranking;
		i = -1;
		do {
			i++;
			ranking -= allmoves[i].ranking;
		} while (ranking >= 0);
		*move = allmoves[i].move;	
	}
	free(data.dptr);
	move->from = standardTox88(move->from);
	move->to = standardTox88(move->to);
	return BOOK_OK;	
}

int addBookMove(Board *board, Move *move) {
	U64 hashkey;
	datum key, data;
	int howmany, i, newmove = 0, failed, found = 0;
	BookMove *allmoves;
	
	if (openingBook == NULL) return BOOK_FAILED;	
	hashkey = getBookHashKey(board);
	key.dsize = sizeof(hashkey);
	key.dptr = (char *) &hashkey;
	data = gdbm_fetch(openingBook,key);
	if (data.dptr == NULL) howmany = 0;
	else howmany = data.dsize / sizeof(BookMove);
	if (howmany == 0) {
		allmoves = (BookMove *) xmalloc(sizeof(BookMove));
		allmoves[0].move.from = x88ToStandard(move->from);
		allmoves[0].move.to = x88ToStandard(move->to);
		allmoves[0].move.piece = move->piece;
		allmoves[0].ranking = 1;
		newmove = 1;
	} else {
		allmoves = (BookMove *) data.dptr;
		for (i = 0; (i < howmany) && (!found); i++) {
			found = ((allmoves[i].move.from == x88ToStandard(move->from)) && 
			(allmoves[i].move.to == x88ToStandard(move->to)) &&
			(allmoves[i].move.piece == move->piece));
		}
		if (i >= howmany) {
			newmove = 1;
			allmoves = (BookMove *) xrealloc(allmoves, sizeof(BookMove) * (howmany + 1));
			allmoves[howmany].move.from = x88ToStandard(move->from);
			allmoves[howmany].move.to = x88ToStandard(move->to);
			allmoves[howmany].move.piece = move->piece;
			allmoves[howmany].ranking = 1;
		} else {
			allmoves[i].ranking++;
		}
	}
	if (newmove) howmany++;
	data.dptr = (char *) allmoves;
	data.dsize = howmany * sizeof(BookMove);
	failed  = gdbm_store(openingBook,key,data,GDBM_REPLACE);
	free(allmoves);
	if (failed) return BOOK_FAILED;
	else return BOOK_OK;
}

char *bookErrorToString(int error) {
	char *temp = NULL;
	switch (error) {
		case BOOK_OK:  break;
		case BOOK_FAILED:  temp = xstrdup("General book failure"); break;
		case BOOK_FAILCREATE: temp = xstrdup("Failed to create opening book"); break;
		case BOOK_FAILREQUIREDINSERT: temp = xstrdup("Failed required insert"); break;
		case BOOK_FAILOPEN: temp = xstrdup("Failed to open opening book"); break;
		case BOOK_WRONGVERSION:  temp = xstrdup("Wrong version of opening book format"); break;

		
		default:  temp = xstrdup("Error unknown");
	}
	return temp;
}

static U64 getBookHashKey(Board * board)
{
	int i, wking;
	U64 key;


	key = 0L;
	wking = (int) WKING;
	i = A1;
	while (i <= H8) {
		key ^= bookkeys[(int) board->squares[i] - wking][x88ToStandard(i)];
		i++;
		if (i & OUT) i+=8;
	}
	if (board->palp[board->ply] >= 0) {
		key ^= bookkeys[PALPBOOKHASHKEY][x88ToStandard(board->palp[board->ply])];
	}
	
	if (!board->wkcastle) {
		key ^= specialkeys[KEYWKCASTLE];
	} 
	if (!board->wqcastle) {
		key ^= specialkeys[KEYWQCASTLE];
	}		
	if (!board->bkcastle) {
		key ^= specialkeys[KEYBKCASTLE];
	} 
	if (!board->bqcastle) {
		key ^= specialkeys[KEYBQCASTLE];
	}		

	if (board->blackturn) {
		key ^= specialkeys[KEYBLACKTURN];
	}
	/*if (board->blackturn) {
	 * key ^= hashtable->blackhash;
	 * } */
	return key;
}

