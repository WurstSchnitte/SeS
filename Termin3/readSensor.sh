#!/bin/bash

pipe=/tmp/sensorTagPipe
echo "Reader is starting"
if [ -z "$1" ];
then
	echo "device addr missing"
	exit 1
fi

if [[ ! -p $pipe ]]; then
    echo "Reader not running"
    exit 1
fi

echo $1

# gatttool -b 24:71:89:19:60:07 --char-write-req --handle=0x3d --value=0100
tmp=$(gatttool -b $1 --char-write-req --handle=0x3f --value=3f00)

err=0
while [ $err -eq 0 ]
do

out=$(gatttool -b $1 --char-read --handle=0x3c)
substr=$(echo $out | cut -d':' -f 2)

	i=0
	err=0
	for digit in ${substr};
	do
		#d=$(echo "ibase=16; "$digit | bc)
		d=$(echo $((0x$digit)))
    		err=$((err + d))
		value[i]=$digit
		#echo -n -e \\x${value[i]} > $pipe
    		i=$((i+1))

		if [ $i -gt 11 ]
		then
			break
		fi
	done
done

echo -n -e \\x${value[0]}\\x${value[1]}\\x${value[2]}\\x${value[3]}\\x${value[4]}\\x${value[5]}\\x${value[6]}\\x${value[7]}\\x${value[8]}\\x${value[9]}\\x${value[10]}\\x${value[11]} > $pipe
