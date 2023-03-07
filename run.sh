#!/bin/sh

cd build
cmake --build .
clear
cd ..
./bin/cpm -k 4 example/test1.txt