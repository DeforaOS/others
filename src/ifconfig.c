/* $Id$ */
/* Copyright (c) 2011-2015 Pierre Pronchery <khorben@defora.org> */
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



#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#if defined(__DeforaOS__)
#elif defined(__FreeBSD__)
# include <ifaddrs.h>
# include <net/if_var.h>
# include <netinet6/in6_var.h>
#elif defined(__NetBSD__)
# include <ifaddrs.h>
# include <net/if.h>
# include <netinet6/in6_var.h>
#endif

#ifndef PROGNAME
# define PROGNAME "ifconfig"
#endif


/* ifconfig */
/* private */
/* types */
typedef int Prefs;
#define PREFS_a	0x1
#define PREFS_m	0x2

typedef struct _idxstr
{
	unsigned int idx;
	char const * str;
} idxstr;


/* prototypes */
static int _ifconfig(Prefs prefs, int argc, char * argv[]);

static int _ifconfig_error(char const * message, int ret);
static int _ifconfig_usage(void);

static char const * _inet_str(struct sockaddr * addr);

static char const * _mac_media_str(int type);
static char const * _mac_status_str(int state);


/* functions */
/* ifconfig */
static int _ifconfig_all(Prefs prefs);
static int _ifconfig_do(Prefs prefs, char const * name, int argc,
		char * argv[]);
static int _ifconfig_show(Prefs prefs, char const * name);
static int _show_mac(Prefs prefs, int fd, struct ifreq * ifr);
#ifdef SIOCGIFFLAGS
static void _show_mac_flags(unsigned short flags);
#endif
static int _mac_media(Prefs prefs, int fd, struct ifreq * ifr);
#ifdef SIOCGIFDATA
static int _mac_status(struct if_data * ifd);
#endif
static int _show_inet(Prefs prefs, int fd, struct ifreq * ifr);
static int _show_inet6(Prefs prefs, char const * name);
#if !defined(__DeforaOS__) && (defined(__NetBSD__) || defined(__FreeBSD__))
static int _inet6_do(Prefs prefs, char const * name, int fd,
		struct ifaddrs * ifa);
#else
static int _inet6_do(Prefs prefs, char const * name, int fd, void * ifa);
#endif
static void _inet6_print_addr(struct in6_addr * addr);

static int _ifconfig(Prefs prefs, int argc, char * argv[])
{
	if(prefs & PREFS_a)
		return _ifconfig_all(prefs);
	if(argc == 1)
		return _ifconfig_show(prefs, argv[0]);
	if(argc > 1)
		return _ifconfig_do(prefs, argv[0], argc - 1, &argv[1]);
	return 0;
}

static int _ifconfig_all(Prefs prefs)
{
	struct if_nameindex * ifni;
	struct if_nameindex * i;
	char const * sep = "";

	if((ifni = if_nameindex()) == NULL)
		return -_ifconfig_error("if_nameindex", 1);
	for(i = ifni; i != NULL && i->if_index != 0; i++)
	{
		fputs(sep, stderr);
		_ifconfig_show(prefs, i->if_name);
		sep = "\n";
	}
	if_freenameindex(ifni);
	return 0;
}

static int _ifconfig_do(Prefs prefs, char const * name, int argc,
		char * argv[])
{
	/* FIXME implement */
	return 0;
}

static int _ifconfig_show(Prefs prefs, char const * name)
{
	int ret;
	int fd;
	struct ifreq ifr;

	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return _ifconfig_error("socket", 1);
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", name);
	ret = _show_mac(prefs, fd, &ifr);
	ret |= _show_inet(prefs, fd, &ifr);
	close(fd);
	ret |= _show_inet6(prefs, name);
	return ret;
}

static int _show_mac(Prefs prefs, int fd, struct ifreq * ifr)
{
	int ret = 0;
#ifdef SIOCGIFDATA
	struct ifdatareq ifi;
#endif

	printf("%s:", ifr->ifr_name);
#ifdef SIOCGIFFLAGS
	if(ioctl(fd, SIOCGIFFLAGS, ifr) != 0)
		ret |= _ifconfig_error("SIOCGIFFLAGS", 1);
	else
		_show_mac_flags(ifr->ifr_flags);
#endif
#ifdef SIOCGIFDATA
	memcpy(ifi.ifdr_name, ifr->ifr_name, sizeof(ifi.ifdr_name));
	if(ioctl(fd, SIOCGIFDATA, &ifi) != 0)
		ret |= -_ifconfig_error("SIOCGIFDATA", 1);
	else
		printf(" mtu %lu", ifi.ifdr_data.ifi_mtu);
#endif
	putchar('\n');
	ret |= _mac_media(prefs, fd, ifr);
#ifdef SIOCGIFDATA
	if(ret == 0)
		ret |= _mac_status(&ifi.ifdr_data);
#endif
	return ret;
}

#ifdef SIOCGIFFLAGS
static void _show_mac_flags(unsigned short flags)
{
	struct
	{
		int flag;
		char const * name;
	} names[] =
	{
# ifdef IFF_UP
		{ IFF_UP,		"UP"		},
# endif
# ifdef IFF_BROADCAST
		{ IFF_BROADCAST,	"BROADCAST"	},
# endif
# ifdef IFF_DEBUG
		{ IFF_DEBUG,		"DEBUG"		},
# endif
# ifdef IFF_LOOPBACK
		{ IFF_LOOPBACK,		"LOOPBACK"	},
# endif
# ifdef IFF_POINTOPOINT
		{ IFF_POINTOPOINT,	"POINTOPOINT"	},
# endif
# ifdef IFF_RUNNING
		{ IFF_RUNNING,		"RUNNING"	},
# endif
# ifdef IFF_NOARP
		{ IFF_NOARP,		"NOARP"		},
# endif
# ifdef IFF_PROMISC
		{ IFF_PROMISC,		"PROMISC"	},
# endif
# ifdef IFF_OACTIVE
		{ IFF_OACTIVE,		"OACTIVE"	},
# endif
# ifdef IFF_SIMPLEX
		{ IFF_SIMPLEX,		"SIMPLEX"	},
# endif
# ifdef IFF_MULTICAST
		{ IFF_MULTICAST,	"MULTICAST"	},
# endif
	};
	size_t i;
	char const * sep = "";

	printf(" flags=%x", flags);
	if(sizeof(names) == 0 || flags == 0)
		return;
	putchar('<');
	for(i = 0; i < sizeof(names) / sizeof(*names); i++)
		if((flags & names[i].flag) == names[i].flag)
		{
			printf("%s%s", sep, names[i].name);
			sep = ",";
		}
	putchar('>');
}
#endif

static int _mac_media(Prefs prefs, int fd, struct ifreq * ifr)
{
#ifdef SIOCGIFMEDIA
	struct ifmediareq ifm;

	memset(&ifm, 0, sizeof(ifm));
	memcpy(ifm.ifm_name, ifr->ifr_name, sizeof(ifm.ifm_name));
	if(ioctl(fd, SIOCGIFMEDIA, &ifm) != 0)
		return (errno != ENOTTY && errno != EINVAL)
			? _ifconfig_error("SIOCGIFMEDIA", 1) : 0;
	printf("\tmedia: %s\n", _mac_media_str(ifm.ifm_current));
#endif
	return 0;
}

#ifdef SIOCGIFDATA
static int _mac_status(struct if_data * ifd)
{
	char const * status;

	if((status = _mac_status_str(ifd->ifi_link_state)) == NULL)
		return 0;
	printf("\tstatus: %s\n", status);
	return 0;
}
#endif

static int _show_inet(Prefs prefs, int fd, struct ifreq * ifr)
{
	if(ioctl(fd, SIOCGIFADDR, ifr) != 0)
	{
#ifdef EADDRNOTAVAIL
		if(errno == EADDRNOTAVAIL)
			return 0;
#endif
#ifdef EAFNOSUPPORT
		if(errno == EAFNOSUPPORT)
			return 0;
#endif
		return -_ifconfig_error("SIOCGIFADDR", 1);
	}
	printf("%s%s", "\tinet: ", _inet_str(&ifr->ifr_addr));
#ifdef SIOCGIFDSTADDR
	if(ioctl(fd, SIOCGIFDSTADDR, ifr) == 0)
		printf(" -> %s", _inet_str(&ifr->ifr_dstaddr));
#endif
#ifdef SIOCGIFBRDADDR
	if(ioctl(fd, SIOCGIFBRDADDR, ifr) == 0)
		printf(" broadcast %s", _inet_str(&ifr->ifr_broadaddr));
#endif
	putchar('\n');
	return 0;
}

static int _show_inet6(Prefs prefs, char const * name)
{
	int ret = 0;
	int fd;
#if defined(__FreeBSD__) || defined(__NetBSD__)
	struct ifaddrs * ifa;
	struct ifaddrs * i;
#endif

	if((fd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
		return -_ifconfig_error("socket", 1);
#if !defined(__DeforaOS__) && (defined(__FreeBSD__) || defined(__NetBSD__))
	if(getifaddrs(&ifa) != 0)
		ret = -_ifconfig_error("getifaddrs", 1);
	else
		for(i = ifa; i != NULL; i = i->ifa_next)
			if(strcmp(i->ifa_name, name) != 0)
				continue;
			else if(i->ifa_addr->sa_family != AF_INET6)
				continue;
			else
				ret |= _inet6_do(prefs, name, fd, i);
#else
	/* FIXME implement */
#endif
	close(fd);
	return ret;
}

#if !defined(__DeforaOS__) && (defined(__FreeBSD__) || defined(__NetBSD__))
static int _inet6_do(Prefs prefs, char const * name, int fd,
		struct ifaddrs * ifa)
{
	struct in6_ifreq ifr;

	if(ifa->ifa_addr->sa_len != sizeof(ifr.ifr_addr))
		return -1;
	memcpy(&ifr.ifr_addr, ifa->ifa_addr, ifa->ifa_addr->sa_len);
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", name);
	printf("%s", "\tinet6: ");
	_inet6_print_addr(&ifr.ifr_ifru.ifru_addr.sin6_addr);
	if(ioctl(fd, SIOCGIFDSTADDR_IN6, &ifr) == 0)
	{
		fputs(" -> ", stdout);
		_inet6_print_addr(&ifr.ifr_ifru.ifru_dstaddr.sin6_addr);
	}
	putchar('\n');
#else
static int _inet6_do(Prefs prefs, char const * name, int fd, void * ifa)
{
	/* FIXME implement */
#endif
	return 0;
}

static void _inet6_print_addr(struct in6_addr * addr)
{
	size_t i;
	char const * sep = "";

	for(i = 0; i < 16; i+=2)
	{
		printf("%s%02x%02x", sep, addr->s6_addr[i],
				addr->s6_addr[i + 1]);
		sep = ":";
	}
}


/* ifconfig_error */
static int _ifconfig_error(char const * message, int ret)
{
	fputs(PROGNAME ": ", stderr);
	perror(message);
	return ret;
}


/* inet_str */
char const * _inet_str(struct sockaddr * addr)
{
	static char buf[16];

	if(addr->sa_family != AF_INET)
		snprintf(buf, sizeof(buf), "%s", "UNKNOWN");
	else
		/* FIXME understand why this is so */
		snprintf(buf, sizeof(buf), "%u.%u.%u.%u",
				(unsigned char)addr->sa_data[2],
				(unsigned char)addr->sa_data[3],
				(unsigned char)addr->sa_data[4],
				(unsigned char)addr->sa_data[5]);
	return buf;
}


/* mac_media_str */
static char const * _mac_media_str(int type)
{
	static char buf[32];
	idxstr is[] =
	{
		{ 0x20,	"Ethernet"	},
		{ 0x80,	"IEEE802.11"	},
		{ 0,	NULL		}
	};
	unsigned int i;

	for(i = 0; is[i].str != NULL; i++)
		if(is[i].idx == (type & 0xe0))
			break;
	if(is[i].str == NULL)
	{
		snprintf(buf, sizeof(buf), "%s (%u)", "UNKNOWN", type);
		return buf;
	}
	snprintf(buf, sizeof(buf), "%s%s", is[i].str, (type & 0x1f) == 0
			? " autoselect" : "");
	return buf;
}


/* mac_status_str */
static char const * _mac_status_str(int state)
{
	switch(state)
	{
#ifdef LINK_STATE_DOWN
		case LINK_STATE_DOWN:		return "inactive";
#endif
#ifdef LINK_STATE_UP
		case LINK_STATE_UP:		return "active";
#endif
#ifdef LINK_STATE_UNKNOWN
		case LINK_STATE_UNKNOWN:
#endif
		default:			return NULL;
	}
}


/* ifconfig_usage */
static int _ifconfig_usage(void)
{
	fputs("Usage: " PROGNAME " [-m] interface [argument...]\n"
"       " PROGNAME " -a\n", stderr);
	return 1;
}


/* public */
/* functions */
/* main */
int main(int argc, char * argv[])
{
	int o;
	Prefs prefs;

	memset(&prefs, 0, sizeof(prefs));
	while((o = getopt(argc, argv, "am")) != -1)
		switch(o)
		{
			case 'a':
				prefs |= PREFS_a;
				break;
			case 'm':
				prefs |= PREFS_m;
				break;
			default:
				return _ifconfig_usage();
		}
	if(prefs & PREFS_a)
	{
		if(optind != argc)
			return _ifconfig_usage();
	}
	else if(optind == argc)
		return _ifconfig_usage();
	return (_ifconfig(prefs, argc - optind, &argv[optind]) == 0) ? 0 : 2;
}
