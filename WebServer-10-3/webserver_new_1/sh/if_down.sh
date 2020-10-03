#!/bin/sh

#author: lliang
#date : 2010-10-12

echo "$1 $2"

INDEX=$1

echo "INDEX = $INDEX"

while [ $INDEX -le $2 ]
do	
	ifconfig eth0:$INDEX down
	echo "ifconfig eth0:$INDEX down"

	let INDEX=$INDEX+1
done
