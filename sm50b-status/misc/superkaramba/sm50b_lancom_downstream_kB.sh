#!/bin/sh
while test -f /tmp/sm50b_superkaramba_lancom.tmp.lock; do sleep 0.5; done
UP=$( cat /tmp/sm50b_superkaramba_lancom.tmp | cut -d \; -f 1 )
UP=$( echo -en "scale=2\\n${UP} / 1024\\n" | bc )
echo "${UP} kB/s"
