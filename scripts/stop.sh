#!/bin/bash
# 
# Stop Nodes IRC.

LIST=`ps -ef | grep "nodes-irc" | grep -v grep | awk '{print $2}'`
if [ ! -z "$LIST" ]; then
	sudo kill -9 $LIST
fi

exit 0
