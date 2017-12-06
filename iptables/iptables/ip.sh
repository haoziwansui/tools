#!/bin/bash

ip_list=(
219.239.227.232
219.239.227.233
219.239.227.234
#112.112.112.112
#value2
#value3
)

####################################################################

#Close all INPUT FORWARD OUTPUT, just open some ports
iptables -P INPUT DROP
iptables -P FORWARD DROP
iptables -P OUTPUT DROP

#Open ssh
iptables -A INPUT -p tcp --dport 22 -j ACCEPT
iptables -A OUTPUT -p tcp --sport 22 -j ACCEPT

#Open port 80
iptables -A INPUT -p tcp --dport 80 -j ACCEPT
iptables -A OUTPUT -p tcp --sport 80 -j ACCEPT


####################################################################

iptables -A INPUT -p tcp --dport 80 -j DROP
iptables -A OUTPUT -p tcp --sport 80 -j DROP
####################################################################


let "len=${#ip_list[@]}-1"

for i in $(seq 0 $len)
do
	#iptables -A INPUT -s ${ip_list[$i]} -j DROP
    iptables -A INPUT -p tcp --dport 80 -s ${ip_list[$i]} -j DROP

done
