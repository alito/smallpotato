#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "libale.h"


#define _GNU_SOURCE
#define _REENTRANT

char *fsafegets(FILE * f, unsigned int chunksize)
{
	char *inbuff, *wholebuff, *newline;
	int curbuflen;
	newline = NULL;
	if (chunksize > 0) {
		wholebuff = (char *) xmalloc(chunksize);
		inbuff = (char *) xmalloc(chunksize);
		curbuflen = 0;
		memset((void *) wholebuff, '\0', chunksize);
		do {
			if (fgets(inbuff, chunksize, f) == NULL) {
				free(wholebuff);
				wholebuff = NULL;
				break;
			}
			curbuflen += chunksize;
			wholebuff = (char *) xrealloc((void *) wholebuff, curbuflen);
			wholebuff = strcat(wholebuff, inbuff);
			newline = strchr(wholebuff, '\n');
		} while ((newline == NULL) && (!feof(f)));
		if ((wholebuff) && (newline != NULL))
			*newline = '\0';
		free(inbuff);
	} else
		wholebuff = NULL;
	return wholebuff;
}

/* return a pointer to a uppercase string version of astring */
char *uppercase(char *astring)
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
char *lowercase(char *astring)
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
char *strip(char *astring)
{
	char *newstring, *startptr, *endptr, *ptr;
	int length;
	if (astring == NULL) {
		return NULL;
	} else {
		for (startptr = astring; *startptr == ' '; startptr++);
		if (*startptr == '\0') {
			newstring = (char *) xmalloc(1);
			*newstring = '\0';
		} else {
			endptr = startptr;
			for (ptr = startptr + 1; *ptr != '\0'; ptr++) {
				if (*ptr != ' ') {
					endptr = ptr;
				}
			}
			length = (int) endptr - (int) startptr + 1;
			newstring = (char *) xmalloc(length + 1);
			memcpy(newstring, startptr, length);
			newstring[length] = '\0';
		}
		return newstring;
	}
}

char *stripall(char *astring, char achar)
{
	char *newstring, *startptr;
	int length, current;
	if ((astring == NULL) || (achar == '\0')) {
		return NULL;
	} else {
		startptr = astring;
		if (*startptr == '\0') {
			newstring = (char *) xmalloc(1);
			*newstring = '\0';
		} else {
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

int
timespec_subtract(struct timespec *result, struct timespec *x,
						struct timespec *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	result->tv_nsec = x->tv_nsec - y->tv_nsec;
	result->tv_sec = x->tv_sec - y->tv_sec;
	while (result->tv_nsec < 0) {
		result->tv_sec--;
		result->tv_nsec += 1000000000;
	}
	return x->tv_sec < y->tv_sec;
}

int parseInteger(char *astring)
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


double parseDouble(char *astring)
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

char *nextToken(char **string, const char *delim)
{
	char *start = *string;
	char *end;
	const char *t;
	int flag = 1;

	if(start == NULL) return NULL;
	 
	while(flag) {
		flag = 0;
		if(*start == '\0') return NULL;
		for(t = delim; *t; t++) {
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
		for(t = delim; *t; t++) {
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
