/* $Id$ */
/* Copyright (c) 2007-2013 Pierre Pronchery <khorben@defora.org> */
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
# define PROGNAME "reboot"
#endif


/* reboot */
/* private */
/* prototypes */
static int _reboot(void);

static int _reboot_error(char const * message, int ret);
static int _reboot_usage(void);


/* functions */
/* reboot */
static int _reboot(void)
{
	sync();
#if defined(RF_REBOOT) /* DeforaOS */
	if(reboot(RF_REBOOT) != 0)
#elif defined(RB_HALT_SYSTEM) /* Linux */
	if(reboot(RB_AUTOBOOT) != 0) /* not a typo */
#elif defined(RB_POWEROFF) /* FreeBSD */
	if(reboot(RB_AUTOBOOT) != 0)
#elif defined(RB_AUTOBOOT)
# if defined(__APPLE__) /* Darwin */
	if(reboot(RB_AUTOBOOT) != 0)
# elif defined(__OpenBSD__) /* OpenBSD */
	if(reboot(RB_AUTOBOOT) != 0)
# else /* NetBSD */
	if(reboot(RB_AUTOBOOT, NULL) != 0)
# endif
#else
# warning Unsupported platform
	errno = ENOSYS;
#endif
		_reboot_error(NULL, 1);
	return 0;
}


/* reboot_error */
static int _reboot_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* reboot_usage */
static int _reboot_usage(void)
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
		return _reboot_usage();
	if(optind != argc)
		return _reboot_usage();
	return _reboot() ? 0 : 2;
}
