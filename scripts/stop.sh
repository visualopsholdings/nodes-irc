#!/bin/bash
# 
# Stop ZMQIRC

LIST=`ps -ef | grep "build/ZMQIRC" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	kill -9 $LIST
fi

exit 0
