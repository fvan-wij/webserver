#!/bin/bash

trap "exit 1" TERM
export TOP_PID=$$


procs_old=9999;
while true
do
	
	if ! ps -p $1 > /dev/null; then
   		kill -s TERM $TOP_PID
	fi

	procs=$(pgrep -P $1 | wc -l)
	if ((procs != procs_old)); then
		echo "child proc count: " $procs
		procs_old=$procs
	fi

done

