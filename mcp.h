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



#ifndef MOOPROXY__HEADER__MCP
#define MOOPROXY__HEADER__MCP



#include "world.h"
#include "line.h"



/* Checks if line is a MCP command. Returns 1 if it is, 0 if not. */
extern int world_is_mcp( char *line );

/* Handle a MCP line from the client. line is consumed. */
extern void world_do_mcp_client( World *wld, Line *line );

/* Handle a MCP line from the server. line is consumed. */
extern void world_do_mcp_server( World *wld, Line *line );

/* Send MCP reset to the server. */
extern void world_send_mcp_reset( World *wld );



#endif  /* ifndef MOOPROXY__HEADER_MCP */