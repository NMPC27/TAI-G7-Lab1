#!/bin/sh

cmake --build build
./bin/cpm "$@" example/chry.txt
