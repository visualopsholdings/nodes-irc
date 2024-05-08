#!/bin/bash
# 
# Start ZMQIRC

if [ ! -f "build/ZMQIRC" ];
then
  echo "Need to build project first"
  exit 1
fi

./build/ZMQIRC > zmqirc.log 2>&1 &
