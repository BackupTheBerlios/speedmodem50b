#!/bin/sh
MINRELOAD=5
MAXRELOAD=900
MINCORRECTION=5
MAXCORRECTION=60
CORRECTIONRELOAD=5

PNGREQUEST=png

if [ "${QUERY_STRING}" != "${PNGREQUEST}" ]; then

  TIMEOUT=${QUERY_STRING}
  [ "$QUERY_STRING" -le "${MAXRELOAD}" ] || QUERY_STRING=${MAXCORRECTION} TIMEOUT=${CORRECTIONRELOAD}
  [ "$QUERY_STRING" -ge "${MINRELOAD}" ]   || QUERY_STRING=${MINCORRECTION} TIMEOUT=${CORRECTIONRELOAD}

  cat <<EOT
Content-Type: text/html

<html>
  <head>
    <meta http-equiv="refresh" content="${TIMEOUT}; URL=${SCRIPT_NAME}?${QUERY_STRING}">
    <title>Connection status of SpeedModem 50B on :  $(date)</title>
  </head>
  <body>
    <pre>$( [ "$TIMEOUT" -ge "${MINRELOAD}" ] && /mybin/sm50b-status/sm50b sm50b | grep -v 'MAC Address' | grep -v '(interleaved)' | grep -A99 'ADSL Status :' | sed 's/(fast)/      /' | head -n 7 )</pre><br>
    <center><img border=0 src="${SCRIPT_NAME}?${PNGREQUEST}"></center>
  </body>
</html>
EOT

else
echo -en 'Content-Type: image/png\n\n'
/mybin/sm50b-status/sm50b sm50b -p
fi

