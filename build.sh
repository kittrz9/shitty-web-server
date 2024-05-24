#!/bin/sh

set -xe

if ! [ -z "$(command -v clang)" ]; then
	CC="clang"
elif ! [ -z "$(command -v gcc)" ]; then
	CC="gcc"
elif [ -z "$CC" ]; then
	echo "no compiler found"
	exit 1
fi

CFLAGS="-O2"

CFILES="$(find src/ -name "*.c")"
CFLAGS="-g -fsanitize=address"
LDFLAGS="$CFLAGS"
OBJS=""
NAME="shittyWebServer"

rm -rf build/ obj/
mkdir build/ obj/

for f in $CFILES; do
	OBJNAME="$(echo $f | sed -e "s/src/obj/;s/\.c/\.o/")"
	$CC $CFLAGS -c $f -o $OBJNAME
	OBJS="$OBJS $OBJNAME"
done


$CC $LDFLAGS $OBJS -o "build/$NAME"
