/* $Id$ */
/* Copyright (c) 2006-2014 Pierre Pronchery <khorben@defora.org> */
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
#include <ctype.h>

#ifndef PROGNAME
# define PROGNAME "hexdump"
#endif


/* hexdump */
/* private */
/* prototypes */
static int _hexdump(int filec, char * filev[]);

static int _hexdump_error(char const * message, int ret);
static int _hexdump_usage(void);


/* functions */
/* hexdump */
static int _hexdump_do(char * filename);

static int _hexdump(int filec, char * filev[])
{
	int ret = 0;
	int i;

	for(i = 0; i < filec; i++)
		ret |= _hexdump_do(filev[i]);
	return ret;
}

static int _hexdump_do(char * filename)
{
	int ret = 0;
	FILE * fp;
	int c;
	int pos = 0;
	char buf[17] = "";

	if((fp = fopen(filename, "r")) == NULL)
		return _hexdump_error(filename, 1);
	while((c = fgetc(fp)) != EOF)
	{
		if(pos % 16 == 0)
			printf("%08x ", pos);
		else if(pos % 16 == 8)
			fputc(' ', stdout);
		printf(" %02x", (unsigned char)c);
		buf[pos % 16] = isprint(c) ? c : '.';
		if(++pos % 16 == 0)
			printf("  |%s|\n", buf);
	}
	buf[pos % 16] = '\0';
	if(pos % 16 != 0)
	{
		if(pos % 16 <= 8)
			fputc(' ', stdout);
		for(c = pos % 16; c < 16; c++)
			printf("%s", "   ");
		printf("  |%s|\n", buf);
	}
	if(!feof(fp))
		ret = _hexdump_error(filename, 1);
	fclose(fp);
	return ret;
}


/* hexdump_error */
static int _hexdump_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* hexdump_usage */
static int _hexdump_usage(void)
{
	fputs("Usage: " PROGNAME " file\n", stderr);
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
				return _hexdump_usage();
		}
	if(optind + 1 != argc)
		return _hexdump_usage();
	return (_hexdump(argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
