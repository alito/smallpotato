#ifndef LIBALE_H
#define LIBALE_H
#include <stdio.h>
#include <time.h>
char *fsafegets(FILE * fileInput, unsigned int chunkSize);
/* like fgets, but it gets until newline is found, increasing buffer size in chunkSize bytes */

char *uppercase(const char *astring); /* return a malloced uppercase version of astring */
char *lowercase(const char *astring); /* return a malloced lowercase version of astring */
char *strip(const char *astring);	  /* return a malloced version of astring stripped
										   * of leading and trailing spaces */
char *stripall(const char *astring, char achar);	/* return a malloced version of astring
															 * stripped of all occurrences of achar */

int parseInteger(const char *astring); /* string to integer (zero on error) */
double parseDouble(const char *astring); /* string to double (zero on error) */

/**
 * Tokenize a string. Borrowed from amy-0.8.1 by
 * Thorsten Greiner (thorsten.greiner@web.de)
 */
char *nextToken(char **string, const char *delimiter);


void *xmalloc(size_t n);
void *xcalloc(size_t n, size_t s);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *p);
#endif
