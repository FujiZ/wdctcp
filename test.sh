#!/bin/sh
make
rmmod wdctcp
insmod wdctcp.ko
sysctl -w net.ipv4.tcp_allowed_congestion_control="cubic reno wdctcp"
