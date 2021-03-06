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
# define PROGNAME "poweroff"
#endif


/* poweroff */
/* private */
/* prototypes */
static int _poweroff(void);

static int _poweroff_error(char const * message, int ret);
static int _poweroff_usage(void);


/* functions */
/* poweroff */
static int _poweroff(void)
{
	sync();
#if defined(RF_HALT) /* DeforaOS */
	if(reboot(RF_POWEROFF) != 0)
#elif defined(__OpenBSD__) /* OpenBSD */
	if(reboot(RB_HALT | RB_POWERDOWN) != 0)
#elif defined(RB_POWERDOWN) /* NetBSD */
	if(reboot(RB_POWERDOWN, NULL) != 0)
#elif defined(RB_POWER_OFF) /* Linux */
	if(reboot(RB_POWER_OFF) != 0)
#else
# warning Unsupported platform
	errno = ENOSYS;
#endif
		return _poweroff_error(NULL, 1);
	return 0;
}


/* poweroff_error */
static int _poweroff_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* poweroff_usage */
static int _poweroff_usage(void)
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
		return _poweroff_usage();
	if(optind != argc)
		return _poweroff_usage();
	return (_poweroff() == 0) ? 0 : 2;
}
