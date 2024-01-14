#!/bin/bash

clang++ -std=c++17 src/server.cpp -o server -pthread 
clang++ -std=c++17 src/client.cpp -o client 


open -a Terminal -n -e "./server"
sleep 2
./client

rm server
rm client