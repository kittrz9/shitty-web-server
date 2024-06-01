#!/bin/sh

set -xe

if [ -z "$(command -v $CC)" ]; then
	if [ -n "$(command -v clang)" ]; then
		CC="clang"
	elif [ -n "$(command -v gcc)" ]; then
		CC="gcc"
	else
		echo "no compiler found"
		exit 1
	fi
fi

if [ -n "$WITH_BZIP2" ]; then
	DEFINES="$DEFINES -DWITH_BZIP2"
	LIBS="$LIBS -lbz2"
fi

if [ -n "$WITH_ZLIB" ]; then
	DEFINES="$DEFINES -DWITH_ZLIB"
	LIBS="$LIBS -lz"
fi

NAME="shittyWebServer"
CFILES="$(find src/ -name "*.c")"
CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -g -fsanitize=address -I src/"
LDFLAGS="$CFLAGS"
DEFINES="$DEFINES -D_POSIX_SOURCE" # have to set this feature test macro thing for the fileno function

OBJS=""

rm -rf build/ obj/
mkdir build/

SRCDIRS="$(find src/ -type d)"
for d in $SRCDIRS; do
	OBJDIR="$(echo "$d" | sed -e "s/src/obj/")"
	mkdir $OBJDIR
done

for f in $CFILES; do
	OBJNAME="$(echo "$f" | sed -e "s/src/obj/;s/\.c/\.o/")"
	$CC $CFLAGS $DEFINES -c "$f" -o $OBJNAME
	OBJS="$OBJS $OBJNAME"
done


$CC $LDFLAGS $OBJS $LIBS -o "build/$NAME"
