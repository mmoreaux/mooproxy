/*
 *
 *  mooproxy - a buffering proxy for moo-connections
 *  Copyright (C) 2001-2005 Marcel L. Moreaux <marcelm@luon.net>
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



#include <time.h>

#include "world.h"
#include "timer.h"
#include "log.h"
#include "misc.h"



static void tick_second( World *, time_t );
static void tick_minute( World *, time_t );
static void tick_hour( World *, time_t );
static void tick_day( World *, time_t, long );
static void tick_month( World *, time_t );
static void tick_year( World *, time_t );



extern void world_timer_tick( World *wld, time_t t )
{
	struct tm *ts;

	ts = localtime( &t );

	/* Seconds */
	if( wld->timer_prev_sec != -1 && wld->timer_prev_sec != ts->tm_sec )
		tick_second( wld, t );

	/* Minutes */
	if( wld->timer_prev_min != -1 && wld->timer_prev_min != ts->tm_min )
		tick_minute( wld, t );

	/* Hours */
	if( wld->timer_prev_hour != -1 && wld->timer_prev_hour != ts->tm_hour )
		tick_hour( wld, t );

	/* Days */
	if( wld->timer_prev_day != -1 && wld->timer_prev_day != ts->tm_mday )
		tick_day( wld, t, ts->tm_year * 366 + ts->tm_yday );

	/* Months */
	if( wld->timer_prev_mon != -1 && wld->timer_prev_mon != ts->tm_mon )
		tick_month( wld, t );

	/* Years */
	if( wld->timer_prev_year != -1 && wld->timer_prev_year != ts->tm_year )
		tick_year( wld, t );

	wld->timer_prev_sec = ts->tm_sec;
	wld->timer_prev_min = ts->tm_min;
	wld->timer_prev_hour = ts->tm_hour;
	wld->timer_prev_day = ts->tm_mday;
	wld->timer_prev_mon = ts->tm_mon;
	wld->timer_prev_year = ts->tm_year;
}



/* Called each time a second elapses. */
static void tick_second( World *wld, time_t t )
{
	set_current_time( t );
}



/* Called each time a minute elapses. */
static void tick_minute( World *wld, time_t t )
{
}



/* Called each time an hour elapses. */
static void tick_hour( World *wld, time_t t )
{
}



/* Called each time a day elapses. */
static void tick_day( World *wld, time_t t, long day )
{
	Line *line;

	set_current_day( day );

	line = world_msg_client( wld, "%s",
			time_string( t, "Day changed to %A %d %b %Y." ) );
	line->flags = LINE_CHECKPOINT;
}



/* Called each time a month elapses. */
static void tick_month( World *wld, time_t t )
{
}



/* Called each time a year elapses. */
static void tick_year( World *wld, time_t t )
{
	world_msg_client( wld, "%s", time_string( t, "Happy %Y!" ) );
}