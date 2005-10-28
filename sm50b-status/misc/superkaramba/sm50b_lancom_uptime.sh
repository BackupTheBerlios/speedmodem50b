#!/bin/sh
wget -O- -o/dev/null http://user:gz76rcd5@dsl/config/1/13/ | grep -A 1 Connections | tail -n 1 | sed 'y/<>/;;/' | cut -d \; -f 9,17 | sed 's/;/ \/ /'
