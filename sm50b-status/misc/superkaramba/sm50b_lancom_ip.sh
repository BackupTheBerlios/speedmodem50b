#!/bin/sh
wget -O- -o/dev/null http://user:gz76rcd5@dsl/config/4/1/3 | grep 'name="value"' | sed 's/<[tb][dr][0-9a-z=\ ]*>//g' | sed 's/<\/[tb][dr][0-9a-z=\ ]*>//g' | cut -d '"' -f 6 | cut -d : -f 2 | grep -v INTERNET
wget -O- -o/dev/null http://user:gz76rcd5@dsl/config/1/6/8/3/ | grep 'IP-address' -A 1 | tail -n 1 | cut -d \> -f 5 | cut -d \< -f 1 | grep -v '0.0.0.0'
