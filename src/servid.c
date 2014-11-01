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
#include <netdb.h>


/* servid */
/* private */
/* prototypes */
static int _servid(char const * protocol, char const * service);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* servid */
static int _servid(char const * protocol, char const * service)
{
	struct servent * se;

	setservent(1);
	if((se = getservbyname(service, protocol)) == NULL)
		return _error(service, 1);
	printf("%d/%s\n", ntohs(se->s_port), se->s_proto);
	return 0;
}


/* error */
static int _error(char const * message, int ret)
{
	fprintf(stderr, "%s: %s: %s\n", "servid", message, "Service not found");
	return ret;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: servid -P protocol service...\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	char const * protocol = NULL;
	int o;
	int i;

	while((o = getopt(argc, argv, "P:")) != -1)
		switch(o)
		{
			case 'P':
				protocol = optarg;
				break;
			default:
				return _usage();
		}
	if(protocol == NULL || optind == argc)
		return _usage();
	for(i = optind; i < argc; i++)
		/* XXX report errors */
		_servid(protocol, argv[i]);
	return 0;
}
