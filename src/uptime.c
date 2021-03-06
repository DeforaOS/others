/* $Id$ */
/* Copyright (c) 2006-2016 Pierre Pronchery <khorben@defora.org> */
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



#if defined(__NetBSD__) && !defined(__DeforaOS__)
# include <sys/param.h>
# include <sys/sysctl.h>
#endif
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifndef PROGNAME
# define PROGNAME "uptime"
#endif

#include "utmpx.c"


/* uptime */
/* private */
/* prototypes */
static int _uptime(void);

static int _uptime_error(char const * message, int ret);
static int _uptime_usage(void);


/* functions */
/* uptime */
static int _uptime(void)
{
	struct timeval tv;
	time_t sec;
	struct tm * tm = NULL;
	char time[9];
	char uptime[9] = "unknown";
#if defined(__NetBSD__) && !defined(__DeforaOS__)
	int name[2] = { CTL_KERN, KERN_BOOTTIME };
	struct timeval uptv;
	size_t s = sizeof(uptv);
#endif
	unsigned int nusers;
	struct utmpx * ut;
	double loadavg[3];

	if((gettimeofday(&tv, NULL)) != 0)
		return _uptime_error("gettimeofday", 1);
	sec = tv.tv_sec;
	if((tm = gmtime(&sec)) == NULL)
		return _uptime_error("gmtime", 1);
	if(strftime(time, sizeof(time), "%X", tm) == 0)
		return _uptime_error("strftime", 1);
	/* FIXME uptime is not portable afaik:
	 * - investigate microuptime(9) */
#if defined(__NetBSD__) && !defined(__DeforaOS__)
	if(sysctl(name, sizeof(name) / sizeof(*name), &uptv, &s, NULL, 0) != 0)
		return _uptime_error("sysctl", 1);
	else
	{
		sec = tv.tv_sec - uptv.tv_sec;
		if((tm = gmtime(&sec)) == NULL)
			return _uptime_error("gmtime", 1);
		/* FIXME also implement days */
		if(strftime(uptime, sizeof(uptime), "%H:%M", tm) == 0)
			return _uptime_error("strftime", 1);
	}
#endif
#ifdef USER_PROCESS
	for(nusers = 0; (ut = getutxent()) != NULL;)
		if(ut->ut_type == USER_PROCESS)
			nusers++;
#else
# warning Unsupported platform: USER_PROCESS is not supported
	nusers = 0;
#endif
	if(getloadavg(loadavg, 3) != 3)
		return _uptime_error("getloadavg", 1);
	printf(" %s up %s, %2d%s%.2f, %.2f, %.2f\n", time, uptime, nusers,
			" users, load average: ", loadavg[0], loadavg[1],
			loadavg[2]);
	return 0;
}


/* uptime_error */
static int _uptime_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* uptime_usage */
static int _uptime_usage(void)
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
		return _uptime_usage();
	if(argc != 1)
		return _uptime_usage();
	return (_uptime() == 0) ? 0 : 2;
}
