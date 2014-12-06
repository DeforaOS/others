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



/* netid */
/* private */
/* prototypes */
static int _netid(char const * network);

static int _netid_error(char const * message, int ret);
static int _netid_usage(void);


/* netid */
static int _netid(char const * network)
{
	struct netent * ne;

	if((ne = getnetbyname(network)) == NULL)
		return -_netid_error(network, 1);
	printf("%u\n", ne->n_net);
	return 0;
}


/* netid_error */
static int _netid_error(char const * message, int ret)
{
	fprintf(stderr, "%s: %s: %s\n", "netid", message, "Network not found");
	return ret;
}


/* netid_usage */
static int _netid_usage(void)
{
	fputs("Usage: netid network...\n", stderr);
	return 1;
}


/* main */
int main(int argc, char * argv[])
{
	int o;
	int i;

	while((o = getopt(argc, argv, "")) != -1)
		switch(o)
		{
			default:
				return _netid_usage();
		}
	if(optind == argc)
		return _netid_usage();
	for(i = optind; i < argc; i++)
		/* XXX report errors */
		_netid(argv[i]);
	return 0;
}
