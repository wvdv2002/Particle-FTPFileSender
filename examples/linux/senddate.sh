#!/bin/bash

if [[ "$#" -lt "4" ]]; then
	echo "I need 4 or 5 inputs: deviceName, startDate in "YYYYMMDD" format, start hour, number of hours (0-999), (logtype: rawlog) "
	echo "Example: $0 test1 20160101 14 5"
	echo "Example2: $0 test1 20170813 12 5 rawlog"
	exit 1
fi 


	
deviceName=$1
startDate=$2
startHour=$3
hours=$4
count=0
totalHours=$((hours+startHour))
logDir='datalog'

if [[ $# -ge "5" ]]; then
	logDir=$5
	echo "Also given logdir, downloading from dir $logDir"

fi


for i in `seq $startHour $totalHours`; do
	count=$((count+1))
	thisDay=`date '+%-d' -d "$startDate+$i hours"`
	thisMonth=`date '+%-m' -d "$startDate+$i hours"`
	thisHour=`date '+%-H' -d "$startDate+$i hours"`
	thisYear=`date '+%Y' -d "$startDate+$i hours"`
	fileString="/$thisYear/$thisMonth/$thisDay/v1-$thisHour.log"
	parFileString="/$logDir$fileString"
	echo "getting file $count of $hours: $parFileString"
	if [ -s "/particle-ftp/$parFileString" ]; then
		echo "file already exists, skipping"
	else
		status=`particle call $millName sendFile $parFileString`
		if [ "$status" != "0" ]; then
			echo "file not found, skipping file, errorcode:$status"
		else
			/home/particle-ftp/waitforxferanduntar.sh
		fi
	fi
	sleep 4
done
echo "done, moving files to $deviceName-$startDate"
mv /particle-ftp/$logDir/ /particle-ftp/$millName-$startDate



