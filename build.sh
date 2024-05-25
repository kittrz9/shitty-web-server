#!/bin/sh

set -xe

if [ -n "$(command -v clang)" ]; then
	CC="clang"
elif [ -n "$(command -v gcc)" ]; then
	CC="gcc"
elif [ -z "$CC" ]; then
	echo "no compiler found"
	exit 1
fi

if [ -n "$WITH_BZIP2" ]; then
	DEFINES="-DWITH_BZIP2"
	LIBS="-lbz2"
fi

NAME="shittyWebServer"
CFILES="$(find src/ -name "*.c")"
CFLAGS="-Wall -Wextra -Wpedantic -g -fsanitize=address"
LDFLAGS="$CFLAGS"

OBJS=""

rm -rf build/ obj/
mkdir build/ obj/

for f in $CFILES; do
	OBJNAME="$(echo "$f" | sed -e "s/src/obj/;s/\.c/\.o/")"
	$CC $CFLAGS $DEFINES -c "$f" -o $OBJNAME
	OBJS="$OBJS $OBJNAME"
done


$CC $LDFLAGS $OBJS $LIBS -o "build/$NAME"
