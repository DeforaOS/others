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



#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


/* main */
int main(void)
{
	const char shell[] = "/bin/sh";
	char * const argv[] = { "sh", "-i", NULL };

	open("/dev/console", O_RDONLY);
	open("/dev/console", O_WRONLY);
	open("/dev/console", O_WRONLY);
	execv(shell, argv);
	perror(shell);
	return errno;
}
