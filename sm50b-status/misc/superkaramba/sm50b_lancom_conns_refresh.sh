#!/bin/sh
touch /tmp/sm50b_superkaramba_lancom_conn.tmp.lock
sleep 0.3
wget -O- -o/dev/null http://user:gz76rcd5@dsl/config/2/8/10/5/ | grep edit_ | wc -l | tee /tmp/sm50b_superkaramba_lancom_conn.tmp
rm -f /tmp/sm50b_superkaramba_lancom_conn.tmp.lock
