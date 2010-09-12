#ifndef LIBALE_H
#define LIBALE_H
#include <stdio.h>
#include <time.h>
char *fsafegets(FILE * f, unsigned int chunksize);
/* like fgets, but it gets until newline is found, increasing buffer size in chunksize bytes */

char *uppercase(char *astring); /* return a malloced uppercase version of astring */
char *lowercase(char *astring); /* return a malloced lowercase version of astring */
char *strip(char *astring);	  /* return a malloced version of astring stripped 
										   * of leading and trailing spaces */
char *stripall(char *astring, char achar);	/* return a malloced version of astring 
															 * stripped of all occurrences of achar */

char *nextToken(char **string, const char *delim);
int parseInteger(char *astring); /* string to integer (zero on error) */
double parseDouble(char *astring); /* string to double (zero on error) */

int
timespec_subtract(struct timespec *result, struct timespec *x,
						struct timespec *y);
/* substract y from x and return in result.  return 1 if y > x */

void *xmalloc(size_t n);
void *xcalloc(size_t n, size_t s);
void *xrealloc(void *p, size_t n);
char *xstrdup(char *p);
#endif
