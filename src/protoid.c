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


/* protoid */
/* private */
/* prototypes */
static int _protoid(char const * name);

static int _protoid_error(char const * message, int ret);
static int _protoid_usage(void);


/* functions */
/* protoid */
static int _protoid(char const * name)
{
	struct protoent * pe;

	setprotoent(1);
	if((pe = getprotobyname(name)) == NULL)
		return _protoid_error(name, 1);
	printf("%d\n", pe->p_proto);
	return 0;
}


/* protoid_error */
static int _protoid_error(char const * message, int ret)
{
	fprintf(stderr, "%s: %s: %s\n", "protoid", message,
			"Protocol not found");
	return ret;
}


/* protoid_usage */
static int _protoid_usage(void)
{
	fputs("Usage: protoid protocol...\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;
	int i;

	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _protoid_usage();
		}
	if(optind == argc)
		return _protoid_usage();
	for(i = optind; i < argc; i++)
		/* XXX report errors */
		_protoid(argv[i]);
	return 0;
}
