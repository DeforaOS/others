/* $Id$ */
/* Copyright (c) 2015 Pierre Pronchery <khorben@defora.org> */
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
#include <signal.h>

#ifndef PROGNAME
# define PROGNAME "resize"
#endif


/* resize */
/* private */
/* prototypes */
static int _resize(void);

static int _resize_error(char const * message, int ret);
static int _resize_usage(void);


/* functions */
/* resize */
static int _resize(void)
{
	pid_t pid;

	pid = getppid();
	if(kill(pid, SIGWINCH) != 0)
		return -_resize_error("kill", 1);
	return 0;
}


/* resize_error */
static int _resize_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* resize_usage */
static int _resize_usage(void)
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
		switch(o)
		{
			default:
				return _resize_usage();
		}
	if(optind != argc)
		return _resize_usage();
	return (_resize() == 0) ? 0 : 2;
}
