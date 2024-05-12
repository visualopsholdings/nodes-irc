#!/bin/bash
# 
# Stop ZMQIRC

LIST=`ps -ef | grep "ZMQIRC" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	sudo kill -9 $LIST
fi

exit 0
