/* $Id$ */
/* Copyright (c) 2008-2014 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* portability */
#if defined(MNT_RDONLY)	/* FreeBSD, NetBSD */
# if !defined(__APPLE__)
#  include <isofs/cd9660/cd9660_mount.h>
# endif
# ifndef MOUNT_CD9660
#  define MT_ISO9660		"cd9660"
# else
#  define MT_ISO9660		MOUNT_CD9660
# endif
# ifndef MOUNT_PROCFS
#  define MT_PROCFS		"procfs"
# else
#  define MT_PROCFS		MOUNT_PROCFS
# endif
# ifndef MNT_NODEV
#  define MF_NODEV		NODEV
# else
#  define MF_NODEV		MNT_NODEV
# endif
# define MF_NOEXEC		MNT_NOEXEC
# define MF_NOSUID		MNT_NOSUID
# define MF_RDONLY		MNT_RDONLY
# if defined(__APPLE__) || defined(__FreeBSD__) || (defined(__NetBSD__) \
			&& (!defined(__NetBSD_Version__) \
				|| __NetBSD_Version__ < 499000000)) /* XXX check */
#  define mount(type, dir, flags, data, data_len) \
	mount(type, dir, flags, data)
# endif
#elif !defined(MT_ISO9660)
# define MF_ASYNC		MS_ASYNC
# define MF_NODEV		MS_NODEV
# define MF_NOEXEC		MS_NOEXEC
# define MF_NOSUID		MS_NOSUID
# define MF_RDONLY		MS_RDONLY
# define MT_ISO9660		"iso9660"
# define MT_PROCFS		"proc"
struct iso_args
{
	char const * fspec;
};
# define mount(type, dir, flags, data, data_len) \
	mount(data ? ((struct iso_args *)data)->fspec : NULL, dir, type, \
				flags, NULL)
# define unmount(dir, flags)	umount(dir)
#endif

/* constants */
#define PACKAGE			"linuxrc"

#define CDROM_PATH		"/mnt/cdrom"
#define DEV_CDROMX		"/dev/cdroms/cdromX"
#define INIT_PATH		CDROM_PATH "/sbin/init"
#define PROC_REAL_ROOT_DEV	"/proc/sys/kernel/real-root-dev"


/* linuxrc_error */
static int _linuxrc_error(char const * message, int ret)
{
	fputs(PACKAGE ": ", stderr);
	perror(message);
	return ret;
}


/* linuxrc_mount_cdrom */
static int _linuxrc_mount_cdrom(char * source, char const * dir)
{
#if defined(MT_ISO9660) && !defined(__APPLE__)
	struct iso_args ia;

	memset(&ia, 0, sizeof(ia));
	ia.fspec = source;
# ifdef DEBUG
	fprintf(stderr, "DEBUG: mount -t %s %s %s\n", MT_ISO9660,
			((struct iso_args *)&ia)->fspec, dir);
# endif
	return mount(MT_ISO9660, dir, MF_NOSUID | MF_NODEV | MF_RDONLY, &ia,
			sizeof(ia));
#else
	errno = ENOSYS;
	return -1;
#endif
}


/* main */
int main(void)
{
	size_t i;
	char dev_cdrom[] = DEV_CDROMX;
	const char dev_root[] = "0x100\n";
	struct stat st;
	int found = 0;
	FILE * fp;

	/* mount /proc */
	if(mount(MT_PROCFS, "/proc", MF_NOEXEC | MF_NOSUID | MF_NODEV, NULL, 0)
			!= 0)
		_linuxrc_error("/proc", 0);
	/* look for the installation CD-ROM */
	for(i = 0; i < 4; i++)
	{
		dev_cdrom[sizeof(dev_cdrom) - 2] = i + '0';
		if(_linuxrc_mount_cdrom(dev_cdrom, CDROM_PATH) != 0)
		{
			_linuxrc_error(dev_cdrom, 0);
			continue;
		}
		if(stat(INIT_PATH, &st) != 0)
		{
			found = 1;
			break;
		}
		_linuxrc_error(INIT_PATH, 0);
		unmount(dev_cdrom, 0); /* FIXME check the flags */
	}
	if(!found)
	{
		fputs(PACKAGE ": Could not find the bootable CD-ROM\n", stderr);
		return 2;
	}
	if((fp = fopen(PROC_REAL_ROOT_DEV, "w")) == NULL)
		return _linuxrc_error(PROC_REAL_ROOT_DEV, 0);
	if(fwrite(dev_root, sizeof(*dev_root), sizeof(dev_root) - 1, fp)
			!= sizeof(dev_root) - 1)
		_linuxrc_error(PROC_REAL_ROOT_DEV, 0);
	if(fclose(fp) != 0)
		_linuxrc_error(PROC_REAL_ROOT_DEV, 0);
	return 0;
}
