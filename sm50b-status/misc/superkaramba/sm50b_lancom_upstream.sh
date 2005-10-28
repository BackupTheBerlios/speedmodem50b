#!/bin/sh
while test -f /tmp/sm50b_superkaramba_lancom.tmp.lock; do sleep 0.5; done
UP=$( cat /tmp/sm50b_superkaramba_lancom.tmp | cut -d \; -f 2 )
echo "${UP} * 100 / ( 448 * 1024 / 8 )" | bc
