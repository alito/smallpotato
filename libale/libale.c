#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "libale.h"


#define _GNU_SOURCE
#define _REENTRANT


/* like fgets, but it gets until newline is found, increasing buffer size in chunkSize bytes */
char *fsafegets(FILE * fileInput, unsigned int chunkSize)
{
	char *inputBuffer, *wholeBuffer = NULL, *newline = NULL;
	int currentBufferLength;

	if (chunkSize > 0) {
		inputBuffer = (char *) xmalloc(chunkSize);
		currentBufferLength = 0;

		do {
			if (fgets(inputBuffer, chunkSize, fileInput) == NULL) {
				// Something ugly happened. Return NULL
				if (wholeBuffer) {
					free(wholeBuffer);
					wholeBuffer = NULL;
				}
				break;
			}
			currentBufferLength += chunkSize;

			if (wholeBuffer) {
				wholeBuffer = (char *) xrealloc((void *) wholeBuffer, currentBufferLength);
				strcat(wholeBuffer, inputBuffer);
			} else {
				// New buffer. Allocate and copy from the input buffer
				wholeBuffer = (char *) xmalloc(currentBufferLength);
				strcpy(wholeBuffer, inputBuffer);
			}
			newline = strchr(wholeBuffer, '\n');
		} while ((newline == NULL) && (!feof(fileInput)));
		if ((wholeBuffer) && (newline != NULL))
			*newline = '\0';
		free(inputBuffer);
	} else // Useless chunkSize
		wholeBuffer = NULL;
	return wholeBuffer;
}

/* return a pointer to a uppercase string version of astring */
char *uppercase(const char *astring)
{
	char *newstring, *ptr;
	if (astring == NULL) {
		return NULL;
	} else {
		newstring = xstrdup(astring);
		for (ptr = newstring; *ptr != '\0'; ptr++) {
			*ptr = toupper(*ptr);
		}
		return newstring;
	}
}


/* return a pointer to a lowercase string version of astring */
char *lowercase(const char *astring)
{
	char *newstring, *ptr;
	if (astring == NULL) {
		return NULL;
	} else {
		newstring = xstrdup(astring);
		for (ptr = newstring; *ptr != '\0'; ptr++) {
			*ptr = tolower(*ptr);
		}
		return newstring;
	}
}


/* strip leading and trailing spaces from a string*/
char *strip(const char *astring)
{
	char *newstring;
	const char* startptr, *endptr, *ptr;
	long length;
	if (astring == NULL) {
		return NULL;
	} else {
		// Find first non-space character
		for (startptr = astring; *startptr == ' '; startptr++);

		// If none there, return an empty string
		if (*startptr == '\0') {
			newstring = (char *) xmalloc(1);
			*newstring = '\0';
		} else {
			endptr = startptr;

			// Find the last non-space character
			for (ptr = startptr + 1; *ptr != '\0'; ptr++) {
				if (*ptr != ' ') {
					endptr = ptr;
				}
			}

			// Copy from first to last non-space character
			length = (long) endptr - (long) startptr + 1;
			newstring = (char *) xmalloc(length + 1);
			memcpy(newstring, startptr, length);
			newstring[length] = '\0';
		}
		return newstring;
	}
}

/* return a malloced version of astring stripped of all occurrences of achar */
char *stripall(const char *astring, char achar)
{
	char *newstring;
	const char* startptr;
	long length, current;

	// Nothing to strip or nothing there
	if ((astring == NULL) || (achar == '\0')) {
		return NULL;
	} else {
		startptr = astring;

		// Empty string in, empty string out
		if (*startptr == '\0') {
			newstring = (char *) xmalloc(1);
			*newstring = '\0';
		} else {

			// Two passes. First one to find out how long a string we need, second to copy the characters across
			length = 0;
			for (startptr = astring; *startptr != '\0'; startptr++) {
				if (*startptr != achar) {
					length++;
				}
			}
			current = 0;
			newstring = (char *) xmalloc(length + 1);
			for (startptr = astring; *startptr != '\0'; startptr++) {
				if (*startptr != achar) {
					newstring[current++] = *startptr;
				}
			}
			newstring[length] = '\0';
		}
		return newstring;
	}
}


/* string to integer (zero on error) */
int parseInteger(const char *astring)
{
	int returnval;
	char *endptr;
	if (astring == NULL) {
		returnval = 0;
	} else {
		returnval = (int) strtol(astring, &endptr, 10);
		/*returnval = atoi(astring); */
	}
	return returnval;
}

/* string to double (zero on error) */
double parseDouble(const char *astring)
{
	double returnval;
	char *endptr;
	if (astring == NULL) {
		returnval = 0;
	} else {
		returnval = (double) strtod(astring, &endptr);
		/*returnval = atoi(astring); */
	}
	return returnval;
}


/**
 * Tokenize a string. Borrowed from amy-0.8.1 by 
 * Thorsten Greiner (thorsten.greiner@web.de)
 */

char *nextToken(char **string, const char *delimiter)
{
	char *start = *string;
	char *end;
	const char *t;
	int flag = 1;

	if(start == NULL) return NULL;
	 
	while(flag) {
		flag = 0;
		if(*start == '\0') return NULL;
		for(t = delimiter; *t; t++) {
			if(*t == *start) {
				flag = 1;
				start++;
				break;
			}
		}
	}
	 
	 end = start+1;
	 
	for(;;) {
		if(*end == '\0') {
			*string = end;
			return start;
		}
		for(t = delimiter; *t; t++) {
			if(*t == *end) {
				*end = 0;
				*string = end+1;
				return start;
		 	}
		}

		end++;
	}

	 /* NEVER REACHED */
}
