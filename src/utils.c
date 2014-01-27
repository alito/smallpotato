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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <libale.h>
#include "board.h"
#include "hash.h"
#include "pieces.h"
#include "eval.h"

static char pieceToChar(pieceType piece)
{
	char rep;
	switch (piece) {
	case BQUEEN:
		rep = 'q';
		break;
	case WQUEEN:
		rep = 'Q';
		break;
	case BBISHOP:
		rep = 'b';
		break;
	case BROOK:
		rep = 'r';
		break;
	case BKNIGHT:
		rep = 'n';
		break;
	case BKING:
		rep = 'k';
		break;
	case BPAWN:
		rep = 'p';
		break;
	case WBISHOP:
		rep = 'B';
		break;
	case WKNIGHT:
		rep = 'N';
		break;
	case WROOK:
		rep = 'R';
		break;
	case WKING:
		rep = 'K';
		break;
	case WPAWN:
		rep = 'P';
		break;
	case NOTHING:
		rep = ' ';
		break;
	default:
		rep = ' ';
	}
	return rep;
}



/* convert a string to a feature */
featureType stringToFeature(char *feature)
{
	featureType returnval;
	if (feature == NULL) {
		returnval = UNKNOWN;
		goto END_BIT;
	}
	if (strcmp(feature, "PING") == 0) {
		returnval = FPING;
		goto END_BIT;
	}
	if (strcmp(feature, "SETBOARD") == 0) {
		returnval = FSETBOARD;
		goto END_BIT;
	}
	if (strcmp(feature, "PLAYOTHER") == 0) {
		returnval = FPLAYOTHER;
		goto END_BIT;
	}
	if (strcmp(feature, "SAN") == 0) {
		returnval = SAN;
		goto END_BIT;
	}
	if (strcmp(feature, "USERMOVE") == 0) {
		returnval = FUSERMOVE;
		goto END_BIT;
	}
	if (strcmp(feature, "TIME") == 0) {
		returnval = FTIME;
		goto END_BIT;
	}
	if (strcmp(feature, "DRAW") == 0) {
		returnval = FDRAW;
		goto END_BIT;
	}
	if (strcmp(feature, "SIGINT") == 0) {
		returnval = FSIGINT;
		goto END_BIT;
	}
	if (strcmp(feature, "SIGTERM") == 0) {
		returnval = FSIGTERM;
		goto END_BIT;
	}
	if (strcmp(feature, "REUSE") == 0) {
		returnval = REUSE;
		goto END_BIT;
	}
	if (strcmp(feature, "ANALYZE") == 0) {
		returnval = FANALYZE;
		goto END_BIT;
	}
	if (strcmp(feature, "MYNAME") == 0) {
		returnval = MYNAME;
		goto END_BIT;
	}
	if (strcmp(feature, "VARIANTS") == 0) {
		returnval = VARIANTS;
		goto END_BIT;
	}
	if (strcmp(feature, "COLORS") == 0) {
		returnval = COLORS;
		goto END_BIT;
	}
	if (strcmp(feature, "ICS") == 0) {
		returnval = FICS;
		goto END_BIT;
	}
	if (strcmp(feature, "NAME") == 0) {
		returnval = FNAME;
		goto END_BIT;
	}
	if (strcmp(feature, "DONE") == 0) {
		returnval = DONE;
		goto END_BIT;
	}
	returnval = UNKNOWN;
 END_BIT:
	return returnval;
}


/* convert a feature to a string */
char *featureToString(featureType feature)
{
	char *astring;
	switch (feature) {
	case FPING:
		astring = xstrdup("PING");
		break;
	case FSETBOARD:
		astring = xstrdup("SETBOARD");
		break;
	case FPLAYOTHER:
		astring = xstrdup("PLAYOTHER");
		break;
	case SAN:
		astring = xstrdup("SAN");
		break;
	case FUSERMOVE:
		astring = xstrdup("USERMOVE");
		break;
	case FTIME:
		astring = xstrdup("TIME");
		break;
	case FDRAW:
		astring = xstrdup("DRAW");
		break;
	case FSIGINT:
		astring = xstrdup("SIGINT");
		break;
	case FSIGTERM:
		astring = xstrdup("SIGTERM");
		break;
	case REUSE:
		astring = xstrdup("REUSE");
		break;
	case FANALYZE:
		astring = xstrdup("ANALYZE");
		break;
	case MYNAME:
		astring = xstrdup("MYNAME");
		break;
	case VARIANTS:
		astring = xstrdup("VARIANTS");
		break;
	case COLORS:
		astring = xstrdup("COLORS");
		break;
	case FICS:
		astring = xstrdup("ICS");
		break;
	case FNAME:
		astring = xstrdup("NAME");
		break;
	case DONE:
		astring = xstrdup("DONE");
		break;
	default:
		astring = xstrdup("UNKNOWN");
	}
	return astring;
}

int parseMinutes(char *astring)
{
	int seconds, minutes, centiseconds;
	char *endptr, **dummy;
	centiseconds = seconds = 0;
	dummy = NULL; /* avoid compiler warnings */
	if (astring != NULL) {
		minutes = (int) strtol(astring, &endptr, 10);
		if ((endptr != NULL) && (endptr != NULL) && (*endptr == ':')) {
			seconds = (int) strtol((endptr) + 1, dummy, 10);

		}
		seconds += minutes * 60;
		centiseconds = seconds * 100;
	}
	return centiseconds;
}

int parseLevel(char *levelString, timeControl * timecontrol)
{
	int returnval;
	char *ptr;
	if ((levelString == NULL) || (timecontrol == NULL)) {
		returnval = -1;
	} else {
		for (ptr = levelString; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			returnval = -1;
			goto ENDPARSE;
		}
		timecontrol->moves = parseInteger(ptr);
		for (; *ptr != '\0' && *ptr != ' '; ptr++)
			if (*ptr == '\0') {
				returnval = -1;
				goto ENDPARSE;
			}
		for (; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			returnval = -1;
			goto ENDPARSE;
		}
		timecontrol->base = parseMinutes(ptr);
		for (; *ptr != '\0' && *ptr != ' '; ptr++)
			if (*ptr == '\0') {
				returnval = -1;
				goto ENDPARSE;
			}
		for (; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			returnval = -1;
			goto ENDPARSE;
		}
		timecontrol->increment = parseInteger(ptr) * 100;
		returnval = 0;
	 ENDPARSE:;
	}
	return returnval;
}

int parseMove(char *astring, Move * move)
{
	int returnval;
	char *stripped;
	returnval = 0;
	if ((astring == NULL) || (move == NULL)) {
		returnval = -1;
	} else {
		stripped = stripall(astring, ' ');
		if (strlen(stripped) < 4) {
			free(stripped);
			returnval = -1;
		} else {
			if ((stripped[0] >= 'A') && (stripped[0] <= 'H')
				 && (stripped[1] >= '1') && (stripped[1] <= '8')) {
				move->from = ((stripped[1] - '1') << 4) + stripped[0] - 'A';
			} else {
				returnval = -1;
			}
			if ((stripped[2] >= 'A') && (stripped[2] <= 'H')
				 && (stripped[3] >= '1') && (stripped[1] <= '8')) {
				move->to = ((stripped[3] - '1') << 4) + stripped[2] - 'A';
			} else {
				returnval = -1;
			}
			if (strlen(stripped) >= 5) {
				switch (stripped[4]) {
				case 'Q':
					move->piece = QUEEN;
					break;
				case 'B':
					move->piece = BISHOP;
					break;
				case 'K':
					move->piece = KING;
					break;
				case 'R':
					move->piece = ROOK;
					break;
				case 'N':
					move->piece = KNIGHT;
					break;
				case 'P':
					move->piece = PAWN;
					break;
				default:
					returnval = -1;

				}
			} else {
				move->piece = NOTHING;
			}
		}
		free(stripped);
	}
	return returnval;
}

void moveToString(Move * themove, char *buffer)
{
	buffer[0] = (char) (x88PosToCol(themove->from)) + 'a';
	buffer[1] = (char) (x88PosToRow(themove->from)) + '1';
	buffer[2] = (char) (x88PosToCol(themove->to)) + 'a';
	buffer[3] = (char) (x88PosToRow(themove->to)) + '1';

	if (themove->piece != NOTHING) {
		switch (themove->piece) {

		case QUEEN:
			buffer[4] = 'q';
			break;
		case KNIGHT:
			buffer[4] = 'n';
			break;
		case ROOK:
			buffer[4] = 'r';
			break;
		case BISHOP:
			buffer[4] = 'b';
			break;
		case PAWN:
			buffer[4] = 'p';
			break;
		case KING:
			buffer[4] = 'k';
			break;
		default:;
		}
		buffer[5] = '\0';
	} else {
		buffer[4] = '\0';
	}

}

int moveToSan(Board *board, Move * themove, char *buffer) {
	pieceType piece, topiece;
	int last, ambi,i, j,islegal, checkambi, ambicolumn, ambirow, fromrow, fromcol, castle,
	palp, legal;
	Move tempmove;
	
	piece = board->squares[themove->from];
	topiece = board->squares[themove->to];
	fromrow = x88PosToRow(themove->from);
	fromcol = x88PosToCol(themove->from);
	checkambi = last = legal = 1;
	castle = palp = 0;
	switch (piece) {
		case WKING: if (themove->from == E1) {
							if (themove->to == G1) {
								buffer[0] = 'O';
								buffer[1] = '-';
								buffer[2] = 'O';
								last = 3;
								castle = 1;
							} else if (themove->to == C1) {
								buffer[0] = 'O';
								buffer[1] = '-';
								buffer[2] = 'O'; 
								buffer[3] = '-';
								buffer[4] = 'O'; 
								last = 5;
								castle = 1;
							}
						}
						if (!castle) {
							buffer[0] = 'K'; 
							checkambi = 0;
						}
						break;
		case BKING: if (themove->from == E8) {
							if (themove->to == G8) {
								buffer[0] = 'O';
								buffer[1] = '-';
								buffer[2] = 'O';
								last = 3;
								castle = 1;
							} else if (themove->to == C8) {
								buffer[0] = 'O';
								buffer[1] = '-';
								buffer[2] = 'O'; 
								buffer[3] = '-';
								buffer[4] = 'O'; 
								last = 5;
								castle = 1;
							}
						}
						if (!castle) {
							buffer[0] = 'K'; 
							checkambi = 0;
						}
						break; 
		case WQUEEN: 
		case BQUEEN: buffer[0] = 'Q'; break;
		case WROOK:
		case BROOK:  buffer[0] = 'R'; break;
		case WBISHOP:
		case BBISHOP:  buffer[0] = 'B'; break;
		case WKNIGHT:
		case BKNIGHT:  buffer[0] = 'N'; break;
		case WPAWN:
		case BPAWN:  if ((topiece != NOTHING) || (themove->to == board->palp[board->ply])) {
							buffer[0] = (char) (fromcol) + 'a';
							last = 1;
							if (themove->to == board->palp[board->ply]) {
								palp = 1;
							}
						} else {
							last = 0;
						}; 
						checkambi = 0;
						break;
		default: buffer[0] = buffer[1] = buffer[2] = 'x';  /* illegal move */
					last = 3;
					legal = 0;
	}
	if ((!castle) && (legal)) {
		if (checkambi) {
			ambi = ambicolumn = ambirow = 0;
			for (j = 0; j < 64; j++) {
				i = standardTox88(j);
				if ((board->squares[i] == piece) && (i != themove->from)) {
					tempmove.to = themove->to;			
					tempmove.piece = themove->piece;
					tempmove.from = i;
					islegal = isLegal(board,&tempmove);
					if (islegal) {
						ambi = 1;
						if (x88PosToRow(i) == fromrow) {
							ambirow = 1;
						}
						if (x88PosToCol(i) == fromcol) {
							ambicolumn = 1;
						}
					}					
				}
			}
			if (ambi) {
				if (!ambicolumn) {
					buffer[last++] = (char) (fromcol) + 'a';
				} else if (!ambirow) {
					buffer[last++] = (char) (fromrow) + '1';
				} else {
					buffer[last++] = (char) (fromcol) + 'a';
					buffer[last++] = (char) (fromrow) + '1';
				}
			}
		}
		if ((topiece != NOTHING) || (palp)) {
			buffer[last++] = 'x';
		}
		buffer[last++] = (char) (x88PosToCol(themove->to)) + 'a';
		buffer[last++] = (char) (x88PosToRow(themove->to)) + '1';
		if (themove->piece != NOTHING) {
			buffer[last++] = '=';
			switch (themove->piece) {
			case QUEEN:
				buffer[last++] = 'Q';
				break;
			case KNIGHT:
				buffer[last++] = 'N';
				break;
			case ROOK:
				buffer[last++] = 'R';
				break;
			case BISHOP:
				buffer[last++] = 'B';
				break;
			case PAWN:
				buffer[last++] = 'P';
				break;
			case KING:
				buffer[last++] = 'K';
				break;
			default:;
			}		
		}
	}
	if (legal) {
		movePieceSimple(board, themove);
		if (inCheck(board)) {
			if (canMove(board)) {
				buffer[last++] = '+';
			} else {
				buffer[last++] = '#';
			}
		}
		unMoveSimple(board);
	}
	buffer[last] = '\0';
	return last;
}

int sanToMove(Board *board, char *san, Move *move) {
	int length, i = 0, lastcoordinate, column, row, gap, candidates, found;
	int locations[10];
	char *equals;
	pieceType piece;
	if (!san) return -1;
	length = strlen(san);
	if (!length) return -1;
	if (san[0] == 'O') {
		move->piece = NOTHING;
		if (length >= 5) {
			if (!strncmp(san,"O-O-O",5)) {
				if (board->blackturn) {
					move->from = E8;
					move->to = C8;
				} else {
					move->from = E1;
					move->to = C1;
				}
			} else if (!strncmp(san,"O-O",3)) {
				if (board->blackturn) {
					move->from = E8;
					move->to = G8;
				} else {
					move->from = E1;
					move->to = G1;
				}				
			} else return -1;
		} else {
			if (!strncmp(san,"O-O",3)) {
				if (board->blackturn) {
					move->from = E8;
					move->to = G8;
				} else {
					move->from = E1;
					move->to = G1;
				}								
			} else return -1;
		}
	} else {
		if (board->blackturn) {
			if (san[0] >= 'a' && san[0] <= 'h') piece = BPAWN;
			else if (san[0] == 'B') piece = BBISHOP;
			else if (san[0] == 'K') piece = BKING;
			else if (san[0] == 'N') piece = BKNIGHT;
			else if (san[0] == 'R') piece = BROOK;
			else if (san[0] == 'Q') piece = BQUEEN;
			else return -1;
		} else {
			if (san[0] >= 'a' && san[0] <= 'h') piece = WPAWN;
			else if (san[0] == 'B') piece = WBISHOP;
			else if (san[0] == 'K') piece = WKING;
			else if (san[0] == 'N') piece = WKNIGHT;
			else if (san[0] == 'R') piece = WROOK;
			else if (san[0] == 'Q') piece = WQUEEN;
			else return -1;
		}
		lastcoordinate = length;
		for (i = 0; i < length; i++) {
			if (san[i] >= 'a' && san[i] <= 'h')
				lastcoordinate = i;
		}
		if (lastcoordinate < length - 1 && san[lastcoordinate + 1] >= '1' && san[lastcoordinate + 1] <= '8') {
			move->to = ((san[lastcoordinate + 1] - '1') << 4) + san[lastcoordinate] - 'a';
		} else return -1;	
		if (piece == WPAWN || piece == BPAWN) {
			equals = strchr(san,'=');
			if (equals == NULL) move->piece = NOTHING;
			else {
				switch (*(equals + 1)) {
					case 'Q': move->piece = QUEEN; break;
					case 'B': move->piece = BISHOP; break;
					case 'R': move->piece = ROOK; break;
					case 'N': move->piece = KNIGHT; break;
					default: return -1;
				}
			}
			if (strchr(san,'x')) {
				if (piece == WPAWN) {
					if (x88PosToCol(move->to) > san[0] - 'a') move->from = move->to - 17;
					else move->from = move->to - 15;
				} else {
					if (x88PosToCol(move->to) > san[0] - 'a') move->from = move->to + 15;
					else move->from = move->to + 17;					
				}
			} else {
				if (piece == WPAWN) {
					i = move->to - 16;
					while (i > 0 && board->squares[i] == NOTHING) i -= 16;
					if (board->squares[i] == WPAWN) move->from = i;
					else return -1;
				} else {
					i = move->to + 16;
					while (i > 0 && board->squares[i] == NOTHING) i += 16;
					if (board->squares[i] == BPAWN) move->from = i;
					else return -1;					
				}
			}
		} else {
			move->piece = NOTHING;
			column = row = -1;
			if (strchr(san,'x')) gap = 1;
			else gap = 0;
			if (lastcoordinate - gap > 1) {
				if (san[1] >= 'a' && san[1] <= 'h') column = san[1] - 'a';
				else if (san[1] >= '1' && san[1] <= '8') row = san[1] - '1';
			}
			candidates = 0;
			for (i = 0; i < 64; i++) {
				if ((board->squares[standardTox88(i)] == piece) && 
				(pieceCanMove(board, standardTox88(i), move->to))) {
					locations[candidates++] = standardTox88(i);
				}
			}
			if (candidates == 0) return -1;
			else if (candidates == 1) move->from = locations[0];
			else if (column >= 0) {
				for (i = 0; i < candidates && x88PosToCol(locations[i]) != column; i++);
				if (i < candidates) move->from = locations[i];
				else return -1;
			} else if (row >= 0) {
				for (i = 0; i < candidates && x88PosToRow(locations[i]) != row; i++);
				if (i < candidates) move->from = locations[i];
				else return -1;				
			} else {
				for (i = found = 0; i < candidates && !found; i++) {
					move->from = locations[i];
					if (isLegal(board,move)) found = 1;
				}
				if (!found) return -1;
			}
		}
	}
	/*printf("san: %s\nfrom %d\tto %d\tpiece %d\n",san,move->from, move->to, move->piece);*/
	return 0;
}

int parseRatings(char *astring, Ratings * ratings)
{
	char *ptr;
	int returnval;
	returnval = -1;
	if ((ratings != NULL) && (astring != NULL)) {
		for (ptr = astring; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			goto ENDRATINGS;
		}
		ratings->ourrating = parseInteger(ptr);
		for (; *ptr != '\0' && *ptr != ' '; ptr++)
			if (*ptr == '\0') {
				goto ENDRATINGS;
			}
		for (; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			goto ENDRATINGS;
		}
		ratings->theirrating = parseInteger(ptr);
		returnval = 0;
	}
 ENDRATINGS:
	return returnval;
}

int parseResult(char *astring, Result * result)
{
	int returnval;
	size_t length;
	char *ptr, *close;
	returnval = 0;
	if ((astring == NULL) || (result == NULL)) {
		returnval = -1;
	} else {
		result->comment = NULL;
		for (ptr = astring; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '\0') {
			returnval = -1;
			goto ENDPARSE;
		}
		if (strncmp(ptr, "1-0", 3) == 0) {
			result->won = WHITEWON;
			goto ENDRESULT;
		}
		if (strncmp(ptr, "0-1", 3) == 0) {
			result->won = BLACKWON;
			goto ENDRESULT;
		}
		if (strncmp(ptr, "1/2-1/2", 7) == 0) {
			result->won = RESDRAW;
			goto ENDRESULT;
		}
		if (*ptr == '*') {
			result->won = UNDECIDED;
		}
	 ENDRESULT:
		for (; *ptr != '\0' && *ptr != ' '; ptr++);
		if (*ptr == '\0') {
			goto ENDPARSE;
		}
		for (; *ptr != '\0' && *ptr == ' '; ptr++);
		if (*ptr == '{') {
			close = strchr(ptr, '}');
			if (close != NULL) {
				length = close - ptr - 1;
				result->comment = (char *) xmalloc(length + 1);
				strncpy(result->comment, ptr + 1, length);
				result->comment[length] = '\0';
			}
		}
	 ENDPARSE:;
	}

	return returnval;
}


#define PIECESMODE 0
#define COLORMODE 1
#define CASTLINGMODE 2
#define PALPMODE 3
#define FIFTYMODE 4
#define MOVECOUNTMODE 5
#define AVOIDMODE 6
#define SOUGHTMOVEMODE 7

static char *parseCommonFENEPD(char *fen, Board *board, int *thepalp) {
	int done, i, mode, empty, square, column, palp;
	char * result;
	if (fen == NULL) return NULL;
	
	done = mode = 0;
	initialiseBoard(board);
	square = A8;  /* initial square reported */
	board->wkcastle = board->bkcastle = board->bqcastle = board->wqcastle = 0;
	palp = 0;
	for (i = 0; i < 128; i++) {
		board->squares[i] = board->colors[i] = NOTHING;
	}
	i = -1;
	do {
		i++;
		switch (mode) {
		case PIECESMODE:
			switch (fen[i]) {
			case 'q':
				board->squares[square] = BQUEEN;
				board->colors[square] = BLACKP;
				break;
			case 'r':
				board->squares[square] = BROOK;
				board->colors[square] = BLACKP;
				break;
			case 'k':
				board->squares[square] = BKING;
				board->colors[square] = BLACKP;
				board->bkingpos = square;
				break;
			case 'b':
				board->squares[square] = BBISHOP;
				board->colors[square] = BLACKP;
				break;
			case 'n':
				board->squares[square] = BKNIGHT;
				board->colors[square] = BLACKP;
				break;
			case 'p':
				board->squares[square] = BPAWN;
				board->colors[square] = BLACKP;
				break;
			case 'Q':
				board->squares[square] = WQUEEN;
				board->colors[square] = WHITEP;
				break;
			case 'R':
				board->squares[square] = WROOK;
				board->colors[square] = WHITEP;
				break;
			case 'K':
				board->squares[square] = WKING;
				board->colors[square] = WHITEP;
				board->wkingpos = square;
				break;
			case 'B':
				board->squares[square] = WBISHOP;
				board->colors[square] = WHITEP;
				break;
			case 'N':
				board->squares[square] = WKNIGHT;
				board->colors[square] = WHITEP;
				break;
			case 'P':
				board->squares[square] = WPAWN;
				board->colors[square] = WHITEP;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				empty = (int) (fen[i] - '0');
				if (x88PosToCol(square) > 8 - empty) {
					done = -1;
				} else {
					square += empty - 1;
				}
				break;
			case '/':
				if (!(square & OUT)) {
					done = -1;
				} else {
					square -= 25; /* jump to one square behind the previous row's start */
				}
				break;
			case ' ':
				if (square != H1 + 1) {
					done = -1;
				} else {
					mode = COLORMODE;
				}
				break;
			default:
				done = -1;
			}
			square++;
			break;
		case COLORMODE:
			switch (fen[i]) {
			case 'w':
			case 'W':
				board->blackturn = 0;
				break;
			case 'b':
			case 'B':
				board->blackturn = 1;
				break;
			case ' ':
				mode = CASTLINGMODE;
				break;
			default:
				done = -1;
			}
			break;

		case CASTLINGMODE:
			switch (fen[i]) {
			case '-':
				break;
			case 'q':
				board->bqcastle = 1;
				break;
			case 'k':
				board->bkcastle = 1;
				break;
			case 'Q':
				board->wqcastle = 1;
				break;
			case 'K':
				board->wkcastle = 1;
				break;
			case ' ':
				mode = PALPMODE;
				break;
			default:
				done = -1;


			}
			break;
		case PALPMODE:
			switch (fen[i]) {
			case '-':
				palp = NOPALP;
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
				column = (int) (fen[i] - 'a');
				i++;
				switch (fen[i]) {
				case '3':
					palp = x88RowColToPos(2, column);
					break;
				case '6':
					palp = x88RowColToPos(5, column);
					break;
				default:
					done = -1;
				}
				break;
			case ' ':
				done = 1;
				break;
			default:
				done = -1;
			}
			break;
		}
	}
	while ((fen[i] != '\0') && (done == 0));
	if (done == 1) {
		*thepalp = palp;
		result = fen + i + 1;  /* return next character */
	} else {
		result = NULL;
	}
	return result;
	
}	

int parseFEN(char *fen, Board * board)
{
	int result, done, i, mode, moves, fifty, palp;
	
	fen = parseCommonFENEPD(fen,board, &palp);
	if (fen == NULL) return -1;
	
	mode = FIFTYMODE;
	done = moves = fifty = 0;
	i = -1; 
	do {
		i++;
		switch (mode) {		
		case FIFTYMODE:
			fifty = 0;
			if ((fen[i] >= '0') && (fen[i] <= '9')) {
				fifty *= 10;
				fifty += (int) (fen[i] - '0');
				if (fifty > 5000) {
					done = -1;
				}
			} else {
				if (fen[i] == ' ') {
					mode = MOVECOUNTMODE;
				} else {
					done = -1;
				}
			}

			break;

		case MOVECOUNTMODE:
			if ((fen[i] >= '0') && (fen[i] <= '9')) {
				moves *= 10;
				moves += (int) (fen[i] - '0');
				if (moves > 5000) {
					done = -1;
				}
			} else {
				if (fen[i] == '\0') {
					done = 1;
				} else {
					done = -1;
				}
			}

			break;
		default:;


		}
	}
	while ((fen[i] != '\0') && (done == 0));

	if (done == 1) {
		board->ply = moves  * 2;
		for (i = 0; i < board->ply - 1; i++) {
			board->history[i] = 0;
			board->palp[i] = NOPALP;
		}
		board->fifty[board->ply] = fifty;
		board->palp[board->ply] = palp;
		board->history[board->ply] = getHashKey(board->hashtable, board);
		checkPhase(board);
		setColorTotals(board);
		result = moves;
	} else {
		result = -1;
	}
	return result;
}

int parseEPD(char *epd, Board * board) {
	int i, palp;
	char *space, *movelist = NULL, *nextspace;
	
	epd = parseCommonFENEPD(epd,board, &palp);
	if (epd == NULL) return FALSE;

	/* find out if we are meant to find or avoid it */
	if (*epd == 'a') board->epdmove.avoid = 1;
	else board->epdmove.avoid = 0;

	/* get the moves to find/avoid */
	space = strchr(epd,' ');
	if (space == NULL) return FALSE;

	while (*space == ' ') space++;
	if (*space == '\0') return FALSE;
	movelist = xstrdup(space);  /* make a temporary copy to play with */
	i = 0;
	space = movelist;
	do {
		nextspace = strchr(space,' ');
		if (nextspace) {
			*nextspace = '\0';
			if (sanToMove(board,space,&(board->epdmove.moves[i])) < 0) return FALSE;
			i++;
			space = nextspace + 1;
			while (*space == ' ') space++;
		} else { /* last iteration */
			if (sanToMove(board,space,&(board->epdmove.moves[i])) < 0) return FALSE;
			i++;
			*space = '\0';  /* hack to break out of loop */
		}
	} while ((*space != '\0') && (i < MAXEPDMOVES));
	board->epdmove.count = i;
	
	board->ply = 100;  /* arbitrary number */
	/* invent fake history */
	for (i = 0; i < board->ply - 1; i++) {
		board->history[i] = 0;
		board->palp[i] = NOPALP;
	}
	board->fifty[board->ply] = 0;
	
	board->palp[board->ply] = palp;
	board->history[board->ply] = getHashKey(board->hashtable, board);
	checkPhase(board);	
	setColorTotals(board);	
	return 1;	
}

#undef PIECESMODE
#undef COLORMODE
#undef CASTLINGMODE
#undef PALPMODE
#undef FIFTYMODE
#undef MOVECOUNTMODE
#undef AVOIDMODE
#undef SOUGHTMOVEMODE



void printResult(FILE * out, Result * result)
{
	if ((out != NULL) && (result != NULL)) {
		char buffer[10];
		fprintf(out,"\n");
		if (result->comment != NULL) {
			fprintf(out, "{%s}", result->comment);
		}		
		switch (result->won) {
		case WHITEWON:
			strcpy(buffer, "1-0");
			break;
		case BLACKWON:
			strcpy(buffer, "0-1");
			break;
		case RESDRAW:
			strcpy(buffer, "1/2-1/2");
			break;
		case UNDECIDED:
			strcpy(buffer, "*");
			break;
		}
		fprintf(out, "%s", buffer);
		fprintf(out, "\n\n");
	}
}


void
printPGNHeader(FILE * out, char *ourname, char *opponentname,
					 int ourrating, int theirrating,
					 timeControl * timecontrol, int playingblack) {
	char *opponent;
	time_t thetime;
	struct tm *timebits;	
	if (out != NULL) {
		fprintf(out,"\n\n[Event \"?\"]\n");
		fprintf(out,"[Site \"?\"]\n");
		time(&thetime);
		timebits = localtime(&thetime);
		fprintf(out,"[Date \"%d.%d.%d\"]\n",timebits->tm_year + 1900, 
				timebits->tm_mon + 1, timebits->tm_mday);
		fprintf(out,"[Time \"%d:%d:%d\"]\n",timebits->tm_hour, 
				timebits->tm_min, timebits->tm_sec);
		fprintf(out,"[Round \"-\"]\n");

		if (opponentname != NULL) {
			opponent = opponentname;
		} else {
			opponent = (char *) xstrdup("Unknown");
		}
		if (playingblack) {
			fprintf(out,"[White \"%s\"]\n",opponent);
			fprintf(out,"[Black \"%s\"]\n",ourname);
			fprintf(out,"[WhiteElo \"%d\"]\n",theirrating);
			fprintf(out,"[BlackElo \"%d\"]\n",ourrating);
			
		} else {
			fprintf(out,"[White \"%s\"]\n",ourname);		
			fprintf(out,"[Black \"%s\"]\n",opponent);
			fprintf(out,"[WhiteElo \"%d\"]\n",ourrating);
			fprintf(out,"[BlackElo \"%d\"]\n",theirrating);
		}
		if (timecontrol != NULL) {
			fprintf(out,"[TimeControl \"");
			if (timecontrol->moves == 0) {
				fprintf(out, "%d",(int) ((timecontrol->base) / 100));
			} else {
				fprintf(out, "%d/%d", timecontrol->moves,(int) ((timecontrol->base) / 100));
			}

			if (timecontrol->increment) {
				fprintf(out, "+%d",(int) ((timecontrol->increment) / 100));
			}
			fprintf(out, "\"]\n");
		} else {
			fprintf(out,"[TimeControl \"*\"]\n");
		}
		fprintf(out,"[Result \"*\"]\n\n");

		if (opponent != opponentname) {
			free(opponent);
		}
	}
}
					 



void
printGameHeader(FILE * out, char *ourname, char *opponentname,
					 int ourrating, int theirrating,
					 timeControl * timecontrol, int playingblack)
{
	char *opponent;
	time_t thetime;
	if (out != NULL) {
		if (opponentname != NULL) {
			opponent = opponentname;
		} else {
			opponent = (char *) xstrdup("Unknown");
		}
		if (playingblack) {
			fprintf(out, "\n%s (%d) vs %s (%d)\n", opponent, theirrating,
					  ourname, ourrating);
		} else {
			fprintf(out, "\n%s (%d) vs %s (%d)\n", ourname, ourrating,
					  opponent, theirrating);
		}
		if (timecontrol != NULL) {
			if (timecontrol->moves == 0) {
				fprintf(out, "All");
			} else {
				fprintf(out, "%d", timecontrol->moves);
			}
			fprintf(out, " in %d seconds", (int) ((timecontrol->base) / 100));
			if (timecontrol->increment) {
				fprintf(out, " with %d second increments",
						  (int) ((timecontrol->increment) / 100));
			}
			fprintf(out, "\n");
		}
		time(&thetime);
		fprintf(out, "%s\n", ctime(&thetime));
		if (opponent != opponentname) {
			free(opponent);
		}
	}
}

void printBoard(FILE * out, Board * board)
{
	pieceType thepiece;
	int count, i, j;
	char rep;
	count = A8;
	if ((out != NULL) && (board != NULL)) {
		fprintf(out, "\n\n-----------------------------------------\n");
		for (i = 0; i < 8; i++) {
			fprintf(out, "|  ");
			for (j = 0; j < 8; j++) {
				thepiece = board->squares[count];
				rep = pieceToChar(thepiece);
				fprintf(out, "%c |  ", rep);
				count++;
			}
			count = count - 24;
			fprintf(out, "\n-----------------------------------------\n");
		}
		fprintf(out, "\n\n");
	}
}


