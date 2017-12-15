#!/bin/sh

echo 'Port-forwading: ip2:port2 => ip1:port1'
echo 'Usage: script.sh ip1 port1 ip2 port2'
ssh -L $1:$2:$3:$4 $USER@$1
