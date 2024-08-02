#!/bin/bash
# 
# Start Nodes IRC.

if [ ! -f "nodes-irc/build/nodes-irc" ];
then
  echo "Need to build or install project first"
  exit 1
fi

# we run this as sudo so that it has access to a certificate that might be in /etc

if [ -d /home/nodes/nodes-lib ];
then
  # for those machines that have local libs on them
  sudo LD_LIBRARY_PATH=/home/nodes/nodes/build:/home/nodes/nodes-lib \
    ./nodes-irc/build/nodes-irc \
    --logLevel=trace \
    > nodes-irc.log 2>&1 &
else
  sudo ./nodes-irc/build/nodes-irc \
    --logLevel=trace \
    > nodes-irc.log 2>&1 &
fi
