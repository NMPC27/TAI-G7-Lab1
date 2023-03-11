#!/bin/sh

cmake --build build
./bin/cpm "$@" example/test1.txt
