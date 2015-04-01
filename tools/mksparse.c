/* $Id$ */
/* Copyright (c) 2015 Pierre Pronchery <khorben@defora.org> */
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



#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* constants */
#define PROGNAME "mksparse"


/* mksparse */
/* private */
/* prototypes */
static int _mksparse(int force, off_t size, int filec, char * const argv[]);

static int _error(char const * message, int ret);
static int _usage(void);


/* functions */
/* mksparse */
static int _mksparse(int force, off_t size, int filec, char * const argv[])
{
	int ret = 0;
	int flags = force ? O_WRONLY | O_CREAT | O_TRUNC
		: O_WRONLY | O_CREAT | O_TRUNC | O_EXCL;
	int i;
	int fd;
	char zero = '\0';

	for(i = 0; i < filec; i++)
	{
		if((fd = open(argv[i], flags, 0666)) < 0)
		{
			ret = _error(argv[i], 2);
			continue;
		}
		if(size > 0 && (lseek(fd, size - 1, SEEK_SET) != size - 1
					|| write(fd, &zero, 1) != 1))
		{
			ret = _error(argv[i], 2);
			unlink(argv[i]);
		}
		if(close(fd) != 0)
			ret = _error(argv[i], 2);
	}
	return ret;
}


/* error */
static int _error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* usage */
static int _usage(void)
{
	fputs("Usage: " PROGNAME " [-f][-s size] file...\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int force = 0;
	int o;
	off_t size = 0;
	char * p;

	while((o = getopt(argc, argv, "fs:")) != -1)
		switch(o)
		{
			case 'f':
				force = 1;
				break;
			case 's':
				size = strtol(optarg, &p, 0);
				if(*optarg == '\0' || *p != '\0')
					return _usage();
				break;
			default:
				return _usage();
		}
	if(optind == argc || size < 0)
		return _usage();
	return (_mksparse(force, size, argc - optind, &argv[optind]) == 0)
		? 0 : 2;
}
