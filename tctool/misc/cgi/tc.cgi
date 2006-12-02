#!/bin/sh
TIMEOUT=60

cat <<EOT
Content-Type: text/html

<html>
  <head>
    <meta http-equiv="refresh" content="${TIMEOUT}; URL=${SCRIPT_NAME}?${QUERY_STRING}">
    <title>Connection status of your SpeedModem 200 on :  $(date)</title>
  </head>
  <body>
    <pre>$( tctool -s 2>/dev/null | grep -v 'MAC Address' )</pre>
    <center><img border=0 src="${SCRIPT_NAME}?${PNGREQUEST}"></center>
  </body>
</html>
EOT
