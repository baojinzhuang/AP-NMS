#!/bin/sh

#author: lliang
#date : 2010-10-12

#./if_conf.sh 172 16 8 59 9 59
#./if_conf.sh 10 39 103 2 1 102

echo "$1 $2 $3 $4 $5 $6"

COUNTER=$4
INDEX=$5

echo "COUNTER = $COUNTER, INDEX = $INDEX"

while [ $COUNTER -le $6 ]
do	
	ifconfig eth0:$INDEX $1.$2.$3.$COUNTER netmask 255.255.0.0
	echo "ifconfig eth0:$INDEX $1.$2.$3.$COUNTER netmask 255.255.0.0"

	let INDEX=$INDEX+1
	let COUNTER=$COUNTER+1
done
