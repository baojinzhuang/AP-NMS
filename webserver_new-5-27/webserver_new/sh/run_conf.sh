#!/bin/sh

#author: lliang
#date : 2010-10-12

#./run_conf.sh 172 16 8 50 51
#./run_conf.sh 10 39 103 2 100


echo "$1 $2 $3 $4 $5"

COUNTER=$4
TOTAL=`expr $4 + $5`

IMSI_VALUE=0
IMSI_POS=13
ISDN_VALUE=0
ISDN_POS=9

echo "COUNTER = $COUNTER, TOTAL = $TOTAL"

while [ $COUNTER -lt $TOTAL ]
do 
	echo "IMSI_VALUE=$IMSI_VALUE, IMSI_POS=$IMSI_POS, ISDN_VALUE=$ISDN_VALUE, ISDN_POS=$ISDN_POS"
	./iuh -l $1.$2.$3.$COUNTER -r 10.39.105.165 -t 10.39.107.245 &
	echo "./iuh -l $1.$2.$3.$COUNTER -r 10.39.105.165 -t 10.39.107.245 &"
	
	let IMSI_VALUE=$IMSI_VALUE+1
	if [ $IMSI_VALUE -gt 9 ]
	then
		let IMSI_VALUE=1
		let IMSI_POS=$IMSI_POS-1
	fi

	let ISDN_VALUE=$ISDN_VALUE+1
	if [ $ISDN_VALUE -gt 9 ]
	then
		let ISDN_VALUE=1
		let ISDN_POS=$ISDN_POS-1
	fi

	let COUNTER=$COUNTER+1
done
