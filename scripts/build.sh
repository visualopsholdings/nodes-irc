#!/bin/bash
# 
# Build the project

if [ ! -d "nodes-irc" ];
then
  echo "nodes-irc project missing."
  exit 1
fi

if [ ! -d "nodes-irc/build" ];
then
  mkdir nodes-irc/build
fi
pushd nodes-irc/build
git pull
cmake ..
make -j
make test
popd
