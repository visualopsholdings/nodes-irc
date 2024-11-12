#!/bin/bash
# 
# Run the cucumber tests

export MONGO_PORT_27017_DB=dev

bundle exec cucumber $1 -r features/step_definitions -r features/support -r build/_deps/nodes-test-src/features 
