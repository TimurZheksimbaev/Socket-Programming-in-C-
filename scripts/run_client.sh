#!/bin/bash

clang++ -arch arm64 src/client/client1.cpp -o client

./client

rm client