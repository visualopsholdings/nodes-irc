#!/bin/bash
# 
# Start Nodes IRC.

if [ ! -f "nodes-irc/build/nodes-irc" ];
then
  echo "Need to build or install project first"
  exit 1
fi

if [ -d /home/nodes/nodes-lib ];
then
  LIBS="LD_LIBRARY_PATH=/usr/local/lib;/home/nodes/nodes/build;/home/nodes/nodes-lib"
fi

sudo $LIBs ./nodes-irc/build/nodes-irc \
  --logLevel=trace \
  > nodes-irc.log 2>&1 &
