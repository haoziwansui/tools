#!/bin/bash

ip_name="10_0_0_0"
round=1
round_end=1

while :
do
	let "round_end=$round+5"
	#sleep 1800
	sleep 5
	tar -cvf ./tar/{$ip_name}_round_{$round}_{$round_end}.tar ./pcap/*
	rm ./pcap/*
	let "round=$round+6"
done
