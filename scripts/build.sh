#!/bin/bash
# 
# Build the project

if [ ! -d "build" ];
then
  mkdir build
fi
pushd build
cmake ..
make
make test
popd
