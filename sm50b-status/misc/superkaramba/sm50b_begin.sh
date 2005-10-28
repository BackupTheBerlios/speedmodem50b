#!/bin/sh
touch /tmp/sm50b_superkaramba.tmp.lock
sleep 0.3
sm50b $1 -s | tee /tmp/sm50b_superkaramba.tmp
rm /tmp/sm50b_superkaramba.tmp.lock
