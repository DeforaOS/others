#!/bin/sh
#$Id$
#Copyright (c) 2011-2015 Pierre Pronchery <khorben@defora.org>
#This file is part of DeforaOS Unix others
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, version 3 of the License.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.



#variables
YEAR=$(date '+%Y')


#functions
#includes
_includes()
{
	BASENAME="$1"
	PROGRAM="$2"

	cat << EOF

/* $BASENAME */
#undef PROGNAME
#define main _${PROGRAM}_main
#define _Prefs _${PROGRAM}_Prefs
#define Prefs ${PROGRAM}_Prefs
#define _prefs_parse _${PROGRAM}_prefs_parse
#include "../src/$BASENAME"
#undef main
#undef _Prefs
#undef Prefs
#undef _prefs_parse
#undef _${PROGRAM}
EOF
}


#toolbox
_toolbox()
{
	target="$1"

	cat > "$target" << EOF
/* \$Id\$ */
/* Copyright (c) 2011-$YEAR Pierre Pronchery <khorben@defora.org> */
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



#include "otherbox.h"

EOF
	for i in ../src/*.c; do
		BASENAME=$(basename "$i")
		PROGRAM=${BASENAME%%.c}
		#there is an exception
		[ "$PROGRAM" = "utmpx" ] && continue
		_includes "$BASENAME" "$PROGRAM"
		CALLS="$CALLS
	{ \"$PROGRAM\",	_${PROGRAM}_main	},"
	done >> "$target"
cat >> "$target" << EOF


Call calls[] =
{
$CALLS
	{ NULL,		NULL		}
};
EOF
}


#main
clean=0
while getopts "cP:" name; do
	case "$name" in
		c)
			clean=1
			;;
		P)
			#we can ignore it
			;;
		?)
			_usage
			exit $?
			;;
	esac
done
shift $(($OPTIND - 1))
if [ $# -ne 1 ]; then
	_usage
	exit $?
fi

[ $clean -ne 0 ] && exit 0

target="$1"
_toolbox "$target"
