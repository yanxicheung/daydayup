#!/bin/sh
regip=$1
recoderfile=/usr/log/ping.txt

echo "=======start=======" >> $recoderfile
local_ip=`ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v inet6|awk '{print $2}'|tr -d "addr:"​`
echo "${local_ip}" >> $recoderfile

while true
do
    ping $regip -c 5 | awk '{ print $0"\t" strftime("%Y-%m-%d %H:%M:%S",systime())}' >> $recoderfile
	sleep 10
done
