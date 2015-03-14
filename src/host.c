/* $Id$ */
/* Copyright (c) 2009-2014 Pierre Pronchery <khorben@defora.org> */
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
#include <netdb.h>
#include <arpa/inet.h>

#ifndef PROGNAME
# define PROGNAME "host"
#endif


/* host */
/* private */
/* prototypes */
static int _host(char * hostname);

static int _host_gaierror(char const * message, int error);
static int _host_usage(void);


/* functions */
/* host */
static int _host(char * hostname)
{
	struct addrinfo * ai;
	int res;
	struct addrinfo * p;
	struct sockaddr_in * sa;

	if((res = getaddrinfo(hostname, NULL, NULL, &ai)) != 0)
		return _host_gaierror(hostname, res);
	for(p = ai; p != NULL; p = p->ai_next)
		if(p->ai_family == AF_INET)
		{
			sa = (struct sockaddr_in *)p->ai_addr;
			printf("%s has address %s\n", hostname,
					inet_ntoa(sa->sin_addr));
		}
	freeaddrinfo(ai);
	return 0;
}


/* host_gaierror */
static int _host_gaierror(char const * message, int error)
{
	fprintf(stderr, "%s%s%s%s\n", PROGNAME ": ", message, ": ",
			gai_strerror(error));
	return -1;
}


/* host_usage */
static int _host_usage(void)
{
	fputs("Usage: " PROGNAME " address\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;

	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _host_usage();
		}
	if(optind != argc - 1)
		return _host_usage();
	return (_host(argv[optind]) == 0) ? 0 : 2;
}
