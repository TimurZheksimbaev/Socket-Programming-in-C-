#!/bin/bash

clang++ -arch arm64 src/client/client.cpp -o client

./client

rm client