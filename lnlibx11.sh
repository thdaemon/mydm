#!/usr/bin/env bash

LDCONFIG=''
libs=()

[ -x "ldconfig" ] && LDCONFIG="ldconfig"
[ -x "/sbin/ldconfig" ] && LDCONFIG="/sbin/ldconfig"
[ -x "/usr/sbin/ldconfig" ] && LDCONFIG="/usr/sbin/ldconfig"

if [ "$LDCONFIG" = "" ]; then
	echo "Can not find ldconfig" >&2
	exit 1
fi

oldIFS=$IFS
IFS=$'\n'
for i in `$LDCONFIG -p | grep "libX11.so.6[\.0-9]*$"` ; do
	i=${i#*=> }
	libs[${#libs[*]}]=$i
done
IFS=$oldIFS

echo "Which libX11 shared object you want to link?\n"

index=0
for i in ${libs[*]} ; do
	echo "$index: $i"
	index=$(($index+1))
done

index=0
read -p "Your choose (default 0): " index
ln -s "${libs[$index]}" ./libX11.so
