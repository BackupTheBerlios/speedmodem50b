#!/bin/sh
touch /tmp/sm50b_superkaramba_lancom.tmp.lock
sleep 0.3
DOWN=$( wget -O- -o/dev/null http://user:gz76rcd5@dsl/config/1/4/8/ | grep -A 1 current | tail -n 1 | sed 'y/<>/;;/' | cut -d \; -f 17,21 | tee /tmp/sm50b_superkaramba_lancom.tmp | cut -d \; -f 1 )
echo "${DOWN} * 100 / ( 3456 * 1024 / 8 )" | bc
rm /tmp/sm50b_superkaramba_lancom.tmp.lock


