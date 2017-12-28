#!/bin/bash

#first call particle call "name" sendFile "path" and afterwards run this script to wait for the transfer to complete and unHeatshrink the file afterwards.
lastLine=`tail -n1 /var/log/proftpd/xferlog`
startTime=`date +%s`
suffix=".lz"
while true; do
	newLine=`tail -n1 /var/log/proftpd/xferlog`
	if [ "$newLine" != "$lastLine" ]; then
		echo "file received"
		break;
	fi
	nowTime=`date +%s`
	runtime=$((nowTime-startTime))
	if (( runtime > 600 )); then
		break;
		echo "timeout"
	fi
	sleep 1	
done
	if [ "$newLine" != "$lastLine" ]; then
	newLine=`tail -n1 /var/log/proftpd/xferlog | cut -d" " -f9`
		if [[ $newLine == *".lz"* ]]; then
			unName=${newLine%$suffix}
			echo "untar $unName"
			heatshrink -d -w 10 -l 4 $newLine $unName > /dev/null 2>&1
			rm $newLine
		fi
	fi
exit 1
