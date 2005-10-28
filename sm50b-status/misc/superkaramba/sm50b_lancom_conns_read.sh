#!/bin/sh
while test -f /tmp/sm50b_superkaramba_lancom_conn.tmp.lock; do sleep 0.5; done
echo $( cat /tmp/sm50b_superkaramba_lancom_conn.tmp ) / 3 | bc
