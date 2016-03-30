/* $Id$ */
/* Copyright (c) 2015-2016 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
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

struct ping_msg
{
	struct
	{
		uint8_t icmp_type;
		uint8_t icmp_code;
		uint16_t icmp_cksum;
		uint16_t icmp_id;
		uint16_t icmp_seq;
	} icmp;
	struct timespec ts;
};


/* constants */
#ifndef ICMP_ECHO
# define ICMP_ECHO	8
#endif


/* prototypes */
static int _ping(Prefs * prefs, char const * hostname);

static int _ping_error(char const * message, int ret);
static int _ping_gaierror(char const * message, int error);
static int _ping_usage(void);


/* functions */
/* ping */
static int _ping_receive(int family, int fd, struct timeval * now);

static int _ping(Prefs * prefs, char const * hostname)
{
	struct addrinfo hints;
	struct addrinfo * ai;
	int res;
	struct addrinfo * to;
	struct ping_msg msg;
	int fd;
	unsigned int i;
	struct timeval tv;
	unsigned int cnt_sent = 0;
	unsigned int cnt_received = 0;
	unsigned int cnt_errors = 0;
	char buf[128];
	struct sockaddr_in * sa;
	struct sockaddr_in6 * sa6;

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
	printf("PING %s", hostname);
	switch(to->ai_family)
	{
		case AF_INET:
			sa = (struct sockaddr_in *)to->ai_addr;
			if(inet_ntop(to->ai_family, &sa->sin_addr, buf,
						sizeof(buf)) != NULL)
				printf(" (%s)", buf);
			break;
		case AF_INET6:
			sa6 = (struct sockaddr_in6 *)to->ai_addr;
			if(inet_ntop(to->ai_family, &sa6->sin6_addr, buf,
						sizeof(buf)) != NULL)
				printf(" (%s)", buf);
			break;
	}
	printf(": %lu data bytes\n", sizeof(msg));
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
			if(_ping_receive(to->ai_family, fd, &tv) == 1)
				cnt_received++;
	}
	printf("%u packets transmitted, %u received, %u errors\n", cnt_sent,
			cnt_received, cnt_errors);
	freeaddrinfo(ai);
	if(close(fd) != 0)
		return _ping_error("close", 1);
	return 0;
}

static int _ping_receive(int family, int fd, struct timeval * now)
{
	fd_set rfds;
	struct timeval tv;
	int res;
	struct ping_msg msg;
	struct sockaddr sa;
	struct sockaddr_in * sin = (struct sockaddr_in *)&sa;
	socklen_t salen;
	ssize_t size;

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if((res = select(fd + 1, &rfds, NULL, NULL, &tv)) == -1)
		return -_ping_error("select", 1);
	else if(res == 0)
		return 0;
	if((size = recvfrom(fd, &msg, sizeof(msg), 0, &sa, &salen)) == -1)
		return -_ping_error("recvfrom", 1);
	/* FIXME really implement */
	if(gettimeofday(&tv, NULL) != 0)
		return -_ping_error("gettimeofday", 1);
	printf("%lu bytes from %s", size, (family == AF_INET)
			? inet_ntoa(sin->sin_addr) : "unknown");
	if(size == sizeof(msg))
		printf(", seq=%u, time=%ldms\n", ntohs(msg.icmp.icmp_seq),
				((tv.tv_sec - now->tv_sec) * 1000)
				+ ((tv.tv_usec >= now->tv_usec)
					? (tv.tv_usec - now->tv_usec)
					: (now->tv_usec - tv.tv_usec)) / 1000);
	else
		printf(" (unknown answer)\n");
	/* wait for the remaining time */
	if(tv.tv_sec > now->tv_sec + 1)
		return 1;
	if(tv.tv_sec == now->tv_sec + 1 && tv.tv_usec >= now->tv_usec)
		return 1;
	tv.tv_sec = 0;
	tv.tv_usec = (tv.tv_usec >= now->tv_usec) ? (tv.tv_usec - now->tv_usec)
		: (now->tv_usec - tv.tv_usec);
	if(select(0, NULL, NULL, NULL, &tv) == -1)
		return -_ping_error("select", 1);
	return 1;
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
