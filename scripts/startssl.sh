#!/bin/bash
# 
# Start ZMQIRC with SSL support.

if [ "$#" -ne 1 ]; then
	echo "usage: $0 KeyDir"
	exit 1
fi

KEYDIR=$1

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

sudo ./zmqirc/build/ZMQIRC --logLevel=trace --port=9999 \
  --chainFile=$KEYDIR/fullchain.pem \
  --certFile=$KEYDIR/privkey.pem \
  > zmqirc.log 2>&1 &
