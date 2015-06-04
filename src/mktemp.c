/* $Id$ */
/* Copyright (c) 2006-2015 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef PROGNAME
# define PROGNAME "mktemp"
#endif


/* mktemp */
/* private */
/* prototypes */
static int _mktemp(char const * template);

static int _mktemp_error(char const * message, int ret);
static int _mktemp_usage(void);


/* functions */
/* mktemp */
static int _mktemp(char const * template)
{
	int ret = 0;
	char * t;
	int fd;
	struct timeval tv;

	if((t = strdup(template)) == NULL)
		return _mktemp_error("strdup", 1);
	if(gettimeofday(&tv, NULL) != 0)
	{
		free(t);
		return _mktemp_error("gettimeofday", 1);
	}
	srand(tv.tv_sec ^ tv.tv_usec ^ getuid() ^ (getpid() << 16));
	if((fd = mkstemp(t)) == -1
			|| close(fd) != 0)
	{
		ret = _mktemp_error(t, 1);
		free(t);
		return ret;
	}
	printf("%s\n", t);
	free(t);
	return ret;
}


/* mktemp_error */
static int _mktemp_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* mktemp_usage */
static int _mktemp_usage(void)
{
	fputs("Usage: " PROGNAME " [-d] [template]\n\
  -d	make a directory instead of a file\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;
	char const * template = P_tmpdir "/tmp.XXXXXX";

	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			case '?':
				return _mktemp_usage();
		}
	if(optind + 1 < argc)
		return _mktemp_usage();
	if(optind + 1 == argc)
		return _mktemp(argv[optind]);
	return (_mktemp(template) == 0) ? 0 : 2;
}
