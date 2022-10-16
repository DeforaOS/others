/* $Id$ */
/* Copyright (c) 2022 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/param.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef PROGNAME
# define PROGNAME "sysctl"
#endif


/* sysctl */
/* private */
/* types */
typedef unsigned int Prefs;
#define PREFS_a  0x1
#define PREFS_v  0x2
#define PREFS_w  0x3

/* prototypes */
static int _sysctl(Prefs prefs, char const * variable);
static int _sysctl_all(Prefs prefs);
static int _sysctl_set(Prefs prefs, char const * variable, char const * value);

static int _usage(void);


/* functions */
/* sysctl */
static int _sysctl(Prefs prefs, char const * variable)
{
	char * buf;
	size_t len = 0;

	if(sysctlbyname(variable, NULL, &len, NULL, 0) != 0)
		switch(errno)
		{
			case ENOMEM:
				break;
			default:
				fprintf(stderr, PROGNAME ": %s: %s\n", variable,
						strerror(errno));
				return -1;
		}
	if((buf = malloc(len)) == NULL)
	{
		fprintf(stderr, PROGNAME ": %s: %s\n", variable,
				strerror(errno));
		return -1;
	}
	if(sysctlbyname(variable, buf, &len, NULL, 0) != 0)
	{
		free(buf);
		fprintf(stderr, PROGNAME ": %s: %s\n", variable,
				strerror(errno));
	}
	/* FIXME print the value */
	free(buf);
	return 0;
}


/* sysctl_all */
static int _sysctl_all(Prefs prefs)
{
	/* FIXME implement */
	return -1;
}


/* sysctl_set */
static int _sysctl_set(Prefs prefs, char const * variable, char const * value)
{
	/* FIXME implement */
	return -1;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " [-v] variable...\n\
       " PROGNAME " -w [-v] variable=value...\n\
       " PROGNAME " -a [-v]\n", stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int ret = 0;
	int o;
	Prefs prefs;
	int i;

	memset(&prefs, 0, sizeof(prefs));
	while((o = getopt(argc, argv, "avw")) != -1)
		switch(o)
		{
			case 'a':
				prefs |= PREFS_a;
				break;
			case 'v':
				prefs |= PREFS_v;
				break;
			case 'w':
				prefs |= PREFS_w;
				break;
			default:
				return _usage();
		}
	if(optind < argc && (prefs & PREFS_a) == 0)
	{
		if((prefs & PREFS_w) == PREFS_w)
		{
			/* FIXME split the parameter on the first '=' */
			for(i = optind; i < argc; i++)
				if(_sysctl_set(prefs, argv[i], NULL) != 0)
					ret = 2;
		}
		else
		{
			for(i = optind; i < argc; i++)
				if(_sysctl(prefs, argv[i]) != 0)
					ret = 2;
		}
	}
	else if(optind == argc && (prefs & PREFS_a) == PREFS_a)
	{
		if(_sysctl_all(prefs) != 0)
			ret = 2;
	}
	else
		return _usage();
	return ret;
}
