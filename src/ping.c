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



#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#if defined(__NetBSD__)
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
#endif

#ifndef PROGNAME
# define PROGNAME "ping"
#endif


/* ping */
/* private */
/* types */
typedef struct _Prefs
{
	int family;
	unsigned int count;
} Prefs;
#define PREFS_c	0x1


/* prototypes */
static int _ping(Prefs * prefs, char const * hostname);

static int _ping_error(char const * message, int ret);
static int _ping_gaierror(char const * message, int error);
static int _ping_usage(void);


/* functions */
/* ping */
static int _ping(Prefs * prefs, char const * hostname)
{
	struct addrinfo hints;
	struct addrinfo * ai;
	int res;
	struct addrinfo * to;
	struct
	{
		struct
		{
			u_int8_t icmp_type;
			u_int8_t icmp_code;
			u_int16_t icmp_cksum;
			union
			{
				struct
				{
					n_short icd_id;
					n_short icd_seq;
				} ih_idseq;
			} icmp_hun;
		} icmp;
		struct timespec ts;
	} msg;
	int fd;
	unsigned int i;
	struct timeval tv;
	unsigned int cnt_sent = 0;
	unsigned int cnt_received = 0;
	unsigned int cnt_errors = 0;

	/* lookup hostname */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = prefs->family;
	hints.ai_socktype = SOCK_STREAM;
	if((res = getaddrinfo(hostname, NULL, &hints, &ai)) != 0)
		return _ping_gaierror(hostname, res);
	for(to = ai; to != NULL; to = to->ai_next)
		if(to->ai_family == AF_INET || to->ai_family == AF_INET6)
			break;
	if(to == NULL)
	{
		fprintf(stderr, "%s: %s: Host not found\n", PROGNAME, hostname);
		freeaddrinfo(ai);
		return -1;
	}
	/* create the socket */
	if((fd = socket(to->ai_family, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		freeaddrinfo(ai);
		return _ping_error("socket", 1);
	}
	/* initialize the packet */
	memset(&msg, 0, sizeof(msg));
	msg.icmp.icmp_type = ICMP_ECHO;
	printf("PING %s (%s): %lu data bytes\n", hostname,
			(to->ai_family == AF_INET) ? inet_ntoa(
				((struct sockaddr_in *)to->ai_addr)->sin_addr)
			: "unknown", sizeof(msg));
	for(i = 0; prefs->count == 0 || i < prefs->count; i++)
	{
		if(gettimeofday(&tv, NULL) != 0)
		{
			_ping_error("gettimeofday", 1);
			continue;
		}
		msg.icmp.icmp_seq = htons(i);
		msg.ts.tv_sec = htonl(tv.tv_sec);
		msg.ts.tv_nsec = htonl(tv.tv_usec);
		/* FIXME set the checksum */
		if(sendto(fd, &msg, sizeof(msg), 0, to->ai_addr, to->ai_addrlen)
				!= sizeof(msg))
			cnt_errors += _ping_error(hostname, 1);
		else
			cnt_sent++;
		if(prefs->count == 0 || i < prefs->count - 1)
			sleep(1);
	}
	printf("%u packets transmitted, %u received, %u errors\n", cnt_sent,
			cnt_received, cnt_errors);
	freeaddrinfo(ai);
	if(close(fd) != 0)
		return _ping_error("close", 1);
	return 0;
}


/* ping_error */
static int _ping_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* ping_gaierror */
static int _ping_gaierror(char const * message, int error)
{
	fprintf(stderr, "%s%s%s%s\n", PROGNAME ": ", message, ": ",
			gai_strerror(error));
	return -1;
}


/* ping_usage */
static int _ping_usage(void)
{
	fputs("Usage: " PROGNAME " [-46][-c count] hostname\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;
	Prefs prefs;
	char * p;

	memset(&prefs, 0, sizeof(prefs));
	prefs.family = AF_UNSPEC;
	prefs.count = 4;
	while((o = getopt(argc, argv, "46c:")) != -1)
		switch(o)
		{
			case '4':
				prefs.family = AF_INET;
				break;
			case '6':
				prefs.family = AF_INET6;
				break;
			case 'c':
				prefs.count = strtoul(optarg, &p, 0);
				if(optarg[0] == '\0' || *p != '\0')
					return _ping_usage();
				break;
			default:
				return _ping_usage();
		}
	if(optind != argc - 1)
		return _ping_usage();
	return (_ping(&prefs, argv[optind]) == 0) ? 0 : 2;
}