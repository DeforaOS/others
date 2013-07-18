#!/bin/sh
# $Id$
# Copyright (c) 2009-2013 Pierre Pronchery <khorben@defora.org>
# This file is part of DeforaOS Unix others
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.



#variables
DELAY=2
SLEEP="sleep"


#functions
#usage
_usage()
{
	echo "Usage: watch command..." 2>&1
	return 1
}


#main
if [ $# -eq 0 -o "$1" = "-h" -o \( $# -eq 1 -a "$1" = "--" \) ]; then
	_usage
	exit $?
fi
[ "$1" = "--" ] && shift 1

while true; do
	clear
	date
	echo
	echo "Every $DELAY seconds: $@"
	echo
	"$@"
	$SLEEP "$DELAY"
done
