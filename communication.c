/*
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
/* Thread communication borrowed from example 5 of the linuxthreads distribution in glibc 2.2 */


#include "common.h"
#include "communication.h"

/* Initialize the action buffer */
void initComms(prodcons * b)
{
	sem_init(&b->sem_write, 0, BUFFER_SIZE);
	sem_init(&b->sem_read, 0, 0);
	b->readpos = 0;
	b->writepos = 0;
}


/* free up semaphores in buffers */
void finaliseComms(prodcons * b)
{
	sem_destroy(&b->sem_write);
	sem_destroy(&b->sem_read);
}

/* Store an action in the buffer */

void putAction(prodcons * b, Action * data)
{
	/* Wait until buffer is not full */
	sem_wait(&b->sem_write);
	/* Write the data and advance write pointer */
	b->buffer[b->writepos] = *data;
	b->writepos++;
	if (b->writepos >= BUFFER_SIZE)
		b->writepos = 0;
	/* Signal that the buffer contains one more element for reading */
	sem_post(&b->sem_read);
}

/* Read and remove an action from the buffer */

void getAction(prodcons * b, Action * data)
{
	/* Wait until buffer is not empty */
	sem_wait(&b->sem_read);
	/* Read the data and advance read pointer */
	*data = b->buffer[b->readpos];
	b->readpos++;
	if (b->readpos >= BUFFER_SIZE)
		b->readpos = 0;
	/* Signal that the buffer has now one more location for writing */
	sem_post(&b->sem_write);
}

/* non-blocking version of putAction */
int tryPutAction(prodcons * b, Action * data)
{
	int returnval;
	/* Check if the buffer is not full */
	returnval = sem_trywait(&b->sem_write);
	if (returnval == 0) {		  /* coast is clear */
		/* Write the data and advance write pointer */
		b->buffer[b->writepos] = *data;
		b->writepos++;
		if (b->writepos >= BUFFER_SIZE)
			b->writepos = 0;
		/* Signal that the buffer contains one more element for reading */
		sem_post(&b->sem_read);
	}
	return returnval;
}


/* non-blocking version of getAction 
	returns 0 if there is something there, a positive number otherwise */
int tryGetAction(prodcons * b, Action * data)
{
	int returnval;
	/* Check if the buffer is not empty */
	returnval = sem_trywait(&b->sem_read);
	if (returnval == 0) {		  /* coast is clear */
		/* Read the data and advance read pointer */
		*data = b->buffer[b->readpos];
		b->readpos++;
		if (b->readpos >= BUFFER_SIZE)
			b->readpos = 0;
		/* Signal that the buffer has now one more location for writing */
		sem_post(&b->sem_write);
	}
	return returnval;
}


/*
int isThereSomethingToRead(prodcons *b) {
	int thecount;
	sem_getvalue(&b->sem_read, &thecount);	
	return (thecount > 0);
}
*/
