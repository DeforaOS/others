/* $Id$ */
/* Copyright (c) 2013-2014 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Unix others */
/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */



#include <unistd.h>
#include <stdio.h>

#ifndef PROGNAME
# define PROGNAME "clear"
#endif


/* clear */
/* private */
/* prototypes */
static int _clear(void);

static int _clear_usage(void);


/* functions */
/* clear */
static int _clear(void)
{
	printf("%c[2J%c[f", 0x1b, 0x1b);
	return 0;
}


/* clear_usage */
static int _clear_usage(void)
{
	fputs("Usage: " PROGNAME "\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;

	while((o = getopt(argc, argv, "")) != -1)
		return _clear_usage();
	if(argc != 1)
		return _clear_usage();
	return (_clear() == 0) ? 0 : 2;
}
