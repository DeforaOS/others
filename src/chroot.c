/* $Id$ */
/* Copyright (c) 2014 Pierre Pronchery <khorben@defora.org> */
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
# define PROGNAME "chroot"
#endif


/* private */
/* prototypes */
static int _chroot(char const * pathname);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* chroot */
static int _chroot(char const * pathname)
{
	if(chroot(pathname) != 0)
		return _error(pathname, 2);
	/* FIXME use the proper shell */
	execl("/bin/sh", "/bin/sh", NULL);
	return 0;
}


/* error */
static int _error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " path\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;

	while((o = getopt(argc, argv, "")) != -1)
		return _usage();
	if(argc - optind != 1)
		return _usage();
	return (_chroot(argv[optind]) == 0) ? 0 : 2;
}