#!/bin/bash
# 
# Start ZMQIRC

if [ ! -f "zmqirc/build/ZMQIRC" ];
then
  echo "Need to build project first"
  exit 1
fi

if [ -d zmqirc/lib ];
then
  # for those machines that have local libs on them
  LD_LIBRARY_PATH=/home/visualops/zmqirc/lib
  export LD_LIBRARY_PATH
fi

./zmqirc/build/ZMQIRC \
  > zmqirc.log 2>&1 &

#./zmqirc/build/ZMQIRC --port=9999 \
# --chainFile=/etc/letsencrypt/live/irc.visualops.com/fullchain.pem \
# --certFile=/etc/letsencrypt/live/irc.visualops.com/privkey.pem \
# > zmqirc.log 2>&1 &
