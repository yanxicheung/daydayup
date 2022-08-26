#!/bin/sh
recoderfile=/usr/log/restart.txt
while true
do
    ps -ef | grep "server_mini" | grep -v grep
    if [ "$?" -eq 1 ];then  #没有这个进程
	
		ps -ef | grep "ping.sh" | grep -v grep
		if [ "$?" -eq 0 ];then #有这个进程
			killall ping.sh
		fi
		
        /usr/bin/run.sh
        echo "restarted!" >> $recoderfile
    fi
	
    sleep 10
done
