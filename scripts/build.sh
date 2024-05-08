#!/bin/bash
# 
# Build the project

if [ ! -d "zmqirc" ];
then
  echo "zmqirc project missing."
  exit 1
fi

if [ ! -d "zmqirc/build" ];
then
  mkdir zmqirc/build
fi
pushd zmqirc/build
cmake ..
make
make test
popd
