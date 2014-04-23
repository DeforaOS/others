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



#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


/* oinit */
/* private */
/* constants */
#define PROGNAME	"oinit"


/* prototypes */
static int _oinit(void);

static int _error(char const * message, int ret);


/* functions */
/* oinit */
static void _oinit_child(void);

static int _oinit(void)
{
	pid_t pid;

	/* spawn a process to initialize the system */
	if((pid = fork()) == -1)
		return -_error("fork", 1);
	else if(pid == 0)
		_oinit_child();
	return 0;
}

static void _oinit_child(void)
{
	const char rc[] = "/etc/rc";
	char * const rc_argv[] = { "rc", NULL };
	const char sh[] = "/bin/sh";
	char * const sh_argv[] = { "sh", "-i", NULL };

	if(setsid() == -1)
		_error("setsid", 1);
	else if(setpgid(0, 0) != 0)
		_error("setpgid", 1);
	/* start /etc/rc */
	execve(rc, rc_argv, NULL);
	_error(rc, 1);
	/* start a backup shell */
	execv(sh, sh_argv);
	_error(sh, 1);
	_exit(errno);
}


/* error */
static int _error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* public */
/* functions */
/* main */
int main(void)
{
	const char console[] = "/dev/console";

	if(getpid() == 1)
	{
		/* open stdin, stdout and stderr on the console */
		if(open(console, O_RDONLY) != 0)
			_error("stdin", 1);
		if(open(console, O_WRONLY) != 1)
			_error("stdout", 1);
		if(open(console, O_WRONLY) != 2)
			_error("stderr", 1);
	}
	_oinit();
	for(;;)
		if(wait(NULL) == -1)
		{
			if(errno == ECHILD)
				/* restart init if there is no process left */
				_oinit();
			else
				/* report the error */
				_error("wait", 1);
		}
	return 0;
}
