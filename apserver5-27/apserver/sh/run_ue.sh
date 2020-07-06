#!/bin/sh

#author: lliang
#date : 2010-10-12

#./run_ue.sh 10 39 103 2 5 0 10 2 40 80

echo "$1 $2 $3 $4 $5 $6 $7 $8 $9 ${10}"

COUNTER=$4
TOTAL=`expr $4 + $5`
AP_INDEX=$6

echo "COUNTER = $COUNTER, TOTAL = $TOTAL, AP_INDEX = $AP_INDEX"

while [ $COUNTER -lt $TOTAL ]
do 
	echo "./iuh -l $1.$2.$3.$COUNTER -r 10.39.105.165 -t 10.39.107.245 -a $AP_INDEX -x $7 -c $8 -s $9 -u ${10} &"
	
	./iuh -l $1.$2.$3.$COUNTER -r 10.39.105.165 -t 10.39.107.245 -a $AP_INDEX -x $7 -c $8 -s $9 -u ${10} &

	let AP_INDEX=$AP_INDEX+1
	let COUNTER=$COUNTER+1
	sleep 1
done
