/*
 *
 *  mooproxy - a smart proxy for MUD/MOO connections
 *  Copyright 2001-2011 Marcel Moreaux
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */



#ifndef MOOPROXY__HEADER__LINE
#define MOOPROXY__HEADER__LINE



#include <time.h>



/* Line flags */
#define LINE_DONTLOG 0x00000001 /* Don't log the line. */
#define LINE_DONTBUF 0x00000002 /* Don't buffer the line. */
#define LINE_NOHIST  0x00000004 /* Don't put the line in history. */
#define LINE_LOGONLY 0x00000008 /* Only send to the log. */

/* Regular server->client or client->server lines. */
#define LINE_REGULAR ( 0 )
/* MCP lines. */
#define LINE_MCP ( LINE_DONTLOG | LINE_DONTBUF | LINE_NOHIST )
/* Mooproxy checkpoint message.
 * These are mooproxy messages that are so important that they
 * deserve to be buffered and logged (e.g. day rollover, shutdown). */
#define LINE_CHECKPOINT ( 0 )
/* Mooproxy normal message (e.g. /settings output). */
#define LINE_MESSAGE ( LINE_DONTLOG | LINE_NOHIST )
/* Recalled lines, like context and possibly-new. */
#define LINE_RECALLED ( LINE_DONTLOG | LINE_DONTBUF | LINE_NOHIST )



/* Line type */
typedef struct Line Line;
struct Line
{
	char *str;
	Line *next;
	Line *prev;
	long len;
	long day;     /* Day of the line's creation. Used in logging. */
	time_t time;  /* Time of the line's creation. */
	int flags;
};

/* Linequeue type */
typedef struct Linequeue Linequeue;
struct Linequeue
{
	Line *head;
	Line *tail;
	/* Number of lines in this queue. */
	unsigned long count;
	/* Total number of bytes this queue occupies. */
	unsigned long size;
};



/* Create a line, set its flags to LINE_REGULAR, and prev/next to NULL.
 * Time and day are set to current time/day.
 * len should contain the length of str (excluding \0), or -1, in which case
 * line_create() will calculate the length itself.
 * Str is consumed. Returns the new line. */
extern Line *line_create( char *str, long len );

/* Destroy line, freeing its resources. */
extern void line_destroy( Line *line );

/* Duplicate line (and its string). All fields are copied, except for
 * prev and next, which are set to NULL. Returns the new line. */
extern Line *line_dup( Line *line );

/* Allocate and initialize a line queue. The queue is empty.
 * Return value: the new queue. */
extern Linequeue *linequeue_create( void );

/* Destroy queue, freeing its resources.
 * Any strings left in the queue are destroyed too. */
extern void linequeue_destroy( Linequeue *queue );

/* Destroy all lines in queue. */
extern void linequeue_clear( Linequeue *queue );

/* Append line to the end of queue */
extern void linequeue_append( Linequeue *queue, Line *line );

/* Remove the line at the start of the queue, and return it (NULL if empty). */
extern Line* linequeue_pop( Linequeue *queue );

/* Remove the line at the end of the queue, and return it (NULL if empty). */
extern Line* linequeue_popend( Linequeue *queue );

/* Remove line from queue, regardless of its position in queue.
 * Line MUST be in queue. Returns line. */
extern Line *linequeue_remove( Linequeue *queue, Line *line );

/* Merge two queues. The contents of queue two is appended to queue one,
 * leaving queue two empty. */
extern void linequeue_merge( Linequeue *one, Linequeue *two );



#endif  /* ifndef MOOPROXY__HEADER__LINE */
