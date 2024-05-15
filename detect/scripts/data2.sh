#!/bin/bash

auto_password(){
	expect << EOF
		spawn scp /home/ivlabs/detect/build/sample.txt rpi@192.168.1.3:/home/rpi/rover
		expect "rpi@192.168.1.3's password:"
		send "rpi\r"
		expect eof
EOF
}

while true
do
	auto_password
	sleep 1
done

