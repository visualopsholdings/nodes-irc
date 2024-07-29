#!/bin/bash
# 
# Start Nodes IRC.

if [ ! -f "nodes-irc/build/nodes-irc" ];
then
  echo "Need to build project first"
  exit 1
fi

if [ -d nodes-irc/lib ];
then
  # for those machines that have local libs on them
  sudo LD_LIBRARY_PATH=/home/visualops/nodes-irc/lib ./nodes-irc/build/nodes-irc \
    --logLevel=trace \
    > nodes-irc.log 2>&1 &
    
# when running without sudo
#   LD_LIBRARY_PATH=/home/visualops/nodes-irc/lib
#   export LD_LIBRARY_PATH
else
  sudo ./nodes-irc/build/nodes-irc \
    --logLevel=trace \
    > nodes-irc.log 2>&1 &
fi

# to run without SSL, comment out the above and just used this.
# BUT DONT!!!!
#
#./nodes-irc/build/nodes-irc \
#  > nodes-irc.log 2>&1 &

