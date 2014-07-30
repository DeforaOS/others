/* $Id$ */
/* Copyright (c) 2007-2014 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/reboot.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#ifndef PROGNAME
# define PROGNAME "halt"
#endif


/* halt */
/* private */
/* prototypes */
static int _halt(void);

static int _halt_error(char const * message, int ret);
static int _halt_usage(void);


/* functions */
/* halt */
static int _halt(void)
{
	sync();
#if defined(RF_HALT) /* DeforaOS */
	if(reboot(RF_HALT) != 0)
#elif defined(RB_HALT_SYSTEM) /* Linux */
	if(reboot(RB_HALT_SYSTEM) != 0)
#elif defined(RB_POWEROFF) /* FreeBSD */
	if(reboot(RB_HALT) != 0)
#elif defined(RB_HALT)
# if defined(__APPLE__) /* Darwin */
	if(reboot(RB_HALT) != 0)
# elif defined(__OpenBSD__) /* OpenBSD */
	if(reboot(RB_HALT) != 0)
# else /* NetBSD */
	if(reboot(RB_HALT, NULL) != 0)
# endif
#else
# warning Unsupported platform
	errno = ENOSYS;
#endif
		return _halt_error(NULL, 1);
	return 0;
}


/* halt_error */
static int _halt_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* halt_usage */
static int _halt_usage(void)
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
		return _halt_usage();
	if(optind != argc)
		return _halt_usage();
	return _halt() ? 0 : 2;
}
