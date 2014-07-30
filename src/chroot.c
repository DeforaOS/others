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
#include <stdlib.h>
#include <stdio.h>

#ifndef PROGNAME
# define PROGNAME "chroot"
#endif


/* private */
/* prototypes */
static int _chroot(char const * pathname);

static int _chroot_error(char const * message, int ret);
static int _chroot_usage(void);


/* functions */
/* chroot */
static int _chroot(char const * pathname)
{
	char const * shell;

	if(chroot(pathname) != 0)
		return _chroot_error(pathname, 2);
	if((shell = getenv("SHELL")) == NULL)
		shell = "/bin/sh";
	execl(shell, shell, NULL);
	return 0;
}


/* chroot_error */
static int _chroot_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* chroot_usage */
static int _chroot_usage(void)
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
		return _chroot_usage();
	if(argc - optind != 1)
		return _chroot_usage();
	return (_chroot(argv[optind]) == 0) ? 0 : 2;
}
