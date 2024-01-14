#!/bin/bash

clang++ -arch arm64 src/server/server.cpp -o server -pthread

./server

rm server