#!/bin/bash

cd ~/detect/build
cmake ..
make

while true; do
	~/detect/build/crater
	sleep 2
done
