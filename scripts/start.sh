#!/bin/bash
# 
# Start ZMQIRC

if [ ! -f "zmqirc/build/ZMQIRC" ];
then
  echo "Need to build project first"
  exit 1
fi

./zmqirc/build/ZMQIRC > zmqirc.log 2>&1 &
