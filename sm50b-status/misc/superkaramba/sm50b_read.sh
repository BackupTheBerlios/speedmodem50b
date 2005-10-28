#!/bin/sh
while test -f /tmp/sm50b_superkaramba.tmp.lock; do sleep 0.5; done
cat /tmp/sm50b_superkaramba.tmp
