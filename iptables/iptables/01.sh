#!/bin/bash

#Allow youself Ping other hosts , prohibit others Ping you
iptables -A INPUT -p icmp --icmp-type 8 -s 0/0 -j DROP
iptables -A OUTPUT -p icmp --icmp-type 8 -s 0/0 -j ACCEPT
