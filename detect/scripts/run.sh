#!/bin/bash

./data.sh &
./camera.sh &

while true;do
	sleep 1
done
