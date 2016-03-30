/* $Id$ */
/* Copyright (c) 2009-2015 Pierre Pronchery <khorben@defora.org> */
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
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifndef PROGNAME
# define PROGNAME "host"
#endif


/* host */
/* private */
/* prototypes */
static int _host(int family, char * hostname);

static int _host_gaierror(char const * message, int error);
static int _host_usage(void);


/* functions */
/* host */
static int _host(int family, char * hostname)
{
	struct addrinfo hints;
	struct addrinfo * ai;
	int res;
	struct addrinfo * p;
	char buf[128];
	struct sockaddr_in * sa;
	struct sockaddr_in6 * sa6;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_STREAM;
	if((res = getaddrinfo(hostname, NULL, &hints, &ai)) != 0)
		return _host_gaierror(hostname, res);
	for(p = ai; p != NULL; p = p->ai_next)
		switch(p->ai_family)
		{
			case AF_INET:
				sa = (struct sockaddr_in *)p->ai_addr;
				if(inet_ntop(p->ai_family, &sa->sin_addr, buf,
							sizeof(buf)) != NULL)
					printf("%s has address %s\n",
							hostname, buf);
				break;
			case AF_INET6:
				sa6 = (struct sockaddr_in6 *)p->ai_addr;
				if(inet_ntop(p->ai_family, &sa6->sin6_addr, buf,
							sizeof(buf)) != NULL)
					printf("%s has IPv6 address %s\n",
							hostname, buf);
				break;
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
	fputs("Usage: " PROGNAME " [-46] address\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;
	int family = AF_UNSPEC;

	while((o = getopt(argc, argv, "46")) != -1)
		switch(o)
		{
			case '4':
				family = AF_INET;
				break;
			case '6':
				family = AF_INET6;
				break;
			default:
				return _host_usage();
		}
	if(optind != argc - 1)
		return _host_usage();
	return (_host(family, argv[optind]) == 0) ? 0 : 2;
}
