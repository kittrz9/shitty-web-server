#!/bin/sh

set -xe

if [ -n "$(command -v tar)" ]; then
	tar --format=ustar -cvf html.tar html/
elif [ -n "$(command -v pax)" ]; then
	pax -w html/ -x ustar > html.pax
else
	echo "no supported archiving program found"
	exit 1
fi
