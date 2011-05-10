#!/bin/sh
DIR=$(dirname $0)
nohup $DIR/irc.pl $1 > /dev/null 2> /dev/null < /dev/null &
sleep 1
