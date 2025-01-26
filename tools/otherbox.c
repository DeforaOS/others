/* $Id$ */
/* Copyright (c) 2011-2025 Pierre Pronchery <khorben@defora.org> */
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "otherbox.h"

/* constants */
#ifndef PROGNAME
# define PROGNAME "otherbox"
#endif


/* otherbox */
/* private */
/* prototypes */
static int _otherbox(char const * name, int argc, char * argv[]);
static int _otherbox_error(char const * message, int ret);
static int _otherbox_list(Call * calls);
static int _otherbox_usage(void);


/* functions */
/* otherbox */
static int _otherbox(char const * name, int argc, char * argv[])
{
	size_t i;

	for(i = 0; calls[i].name != NULL; i++)
		if(strcmp(calls[i].name, name) == 0)
			return calls[i].call(argc, argv);
	return -1;
}


/* otherbox_error */
static int _otherbox_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* otherbox_list */
static int _otherbox_list(Call * calls)
{
	size_t i;

	for(i = 0; calls[i].name != NULL; i++)
		puts(calls[i].name);
	return 0;
}


/* otherbox_usage */
static int _otherbox_usage(void)
{
	fputs("Usage: " PROGNAME " program [arguments...]\n"
"       " PROGNAME " -l\n"
"  -l	List available programs\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int ret;
	char * p;
	char const * q;
	int o;

	if((p = strdup(argv[0])) == NULL)
		return _otherbox_error(NULL, 2);
	q = basename(p);
	ret = _otherbox(q, argc, argv);
	free(p);
	if(ret >= 0)
		return ret;
	while((o = getopt(argc, argv, "l")) != -1)
		switch(o)
		{
			case 'l':
				return _otherbox_list(calls);
			default:
				return _otherbox_usage();
		}
	if(optind == argc)
		return _otherbox_usage();
	if((ret = _otherbox(argv[optind], argc - optind, &argv[optind])) >= 0)
		return ret;
	fprintf(stderr, "%s: %s: command not found\n", PROGNAME, argv[optind]);
	return 127;
}
