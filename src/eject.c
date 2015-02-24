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



#if !defined(__DeforaOS__) && defined(__NetBSD__)
# include <sys/types.h>
# include <sys/cdio.h>
# include <sys/ioctl.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#ifndef PROGNAME
# define PROGNAME "eject"
#endif


/* eject */
/* private */
/* prototypes */
static int _eject(char const * device);

static int _eject_error(char const * message, int ret);
static int _eject_usage(void);


/* functions */
/* eject */
static int _eject(char const * device)
{
	int fd;

	if((fd = open(device, O_RDONLY)) < 0)
		return -_eject_error(device, errno);
#if defined(DIOCEJECT)
	if(ioctl(fd, CDIOCEJECT, 0) != 0)
		_eject_error(device, 0);
#endif
	if(close(fd) != 0)
		return -_eject_error(device, errno);
	return 0;
}


/* eject_error */
static int _eject_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* eject_usage */
static int _eject_usage(void)
{
	fputs("Usage: " PROGNAME " device...\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int ret = 0;
	int o;

	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _eject_usage();
		}
	if(optind == argc)
		return _eject_usage();
	for(o = optind; o < argc; o++)
		ret |= _eject(argv[o]);
	return (ret == 0) ? 0 : 2;
}
