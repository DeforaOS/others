/* $Id$ */
/* Copyright (c) 2007-2022 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/statvfs.h>
#ifdef ST_WAIT /* NetBSD */
# include <sys/param.h>
#endif
#include <sys/mount.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef PROGNAME
# define PROGNAME "umount"
#endif


/* umount */
/* private */
/* types */
typedef unsigned int Prefs;
#define PREFS_a 0x1
#define PREFS_f 0x2


/* prototypes */
static int _umount(Prefs * prefs, int pathc, char * pathv[]);

static int _umount_error(char const * message, int ret);
static int _umount_usage(void);


/* functions */
/* umount */
static int _umount_all(Prefs * prefs);
static int _umount_do(Prefs * prefs, char const * pathname);

static int _umount(Prefs * prefs, int pathc, char * pathv[])
{
	int ret = 0;
	int i;

	if(*prefs & PREFS_a && pathc == 0)
		return _umount_all(prefs);
	for(i = 0; i < pathc; i++)
		ret |= _umount_do(prefs, pathv[i]);
	return ret;
}

#ifdef ST_WAIT
static int _umount_all(Prefs * prefs)
{
	int ret = 0;
	int cnt;
	struct statvfs * f;
	int i;

	if((cnt = getvfsstat(NULL, 0, ST_WAIT)) < 0)
		return _umount_error("getvfsstat", 1);
	if((f = malloc(sizeof(*f) * cnt)) == NULL)
		return _umount_error("malloc", 1);
	if((cnt = getvfsstat(f, sizeof(*f) * cnt, ST_WAIT)) < 0)
	{
		free(f);
		return _umount_error("getvfsstat", 1);
	}
	for(i = cnt - 1; i >= 0; i--)
		if(strcmp("/", f[i].f_mntonname) == 0)
			continue;
		else
			ret |= _umount_do(prefs, f[i].f_mntonname);
	free(f);
	return ret;
#else
# include <errno.h>
static int _umount_all(Prefs * prefs)
{
	int ret = 0;
	char const path[] = "/proc/mounts";
	FILE * fp;
	unsigned char buf[1024];
	size_t i;
	size_t j;
	int c;

	if((fp = fopen(path, "r")) == NULL)
		return -_umount_error(path, 1);
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		/* skip device */
		for(i = 0; buf[i] != '\0' && !isspace((c = buf[i])); i++);
		for(; isspace((c = buf[i])); i++);
		/* determine mountpoint */
		for(j = i; buf[j] != '\0' && !isspace((c = buf[j])); j++);
		if(j > i && buf[j] != '\0' && strncmp(&buf[i], "/", j - i) != 0)
		{
			buf[j++] = '\0';
			ret |= _umount_do(prefs, &buf[i]);
		}
		for(; buf[j] != '\0' && buf[j] != '\n'; j++);
		if(buf[j] == '\n')
			continue;
		/* flush longer lines */
		for(c = fgetc(fp); c != EOF && c != '\n'; c = fgetc(fp));
	}
	if(ferror(fp))
		ret |= -_umount_error(path, 1);
	fclose(fp);
	return ret;
#endif
}

static int _umount_do(Prefs * prefs, char const * pathname)
{
	int flags = 0;

#ifdef MNT_FORCE
	if(*prefs & PREFS_f)
		flags |= MNT_FORCE;
#endif
#ifdef MS_RDONLY /* Linux */
	if(umount(pathname) == 0)
#else
	if(unmount(pathname, flags) == 0)
#endif
		return 0;
	return _umount_error(pathname, 1);
}


/* umount_error */
static int _umount_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* umount_usage */
static int _umount_usage(void)
{
	fputs("Usage: " PROGNAME " -a [-f]\n"
"       " PROGNAME " [-f] special | node ...\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	Prefs prefs;
	int o;

	prefs = 0;
	while((o = getopt(argc, argv, "af")) != -1)
		switch(o)
		{
			case 'a':
				prefs |= PREFS_a;
				break;
			case 'f':
				prefs |= PREFS_f;
				break;
			default:
				return _umount_usage();
		}
	if(optind == argc && (prefs & PREFS_a) != PREFS_a)
		return _umount_usage();
	return (_umount(&prefs, argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
