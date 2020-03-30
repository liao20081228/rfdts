#!/bin/bash
if [ $# -lt 1 ]
then
	echo "please input freqency"
	exit
fi
cpufreq-info
CPUCORES=$(cat /proc/cpuinfo | grep "processor" | wc -l)
for (( i=0; i<$CPUCORES; i++ ))
do

	sudo cpufreq-set -d $1 -u $1 -c $i
done
sleep 3
cpufreq-info
