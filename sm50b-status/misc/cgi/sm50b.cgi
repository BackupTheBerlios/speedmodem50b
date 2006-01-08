#!/bin/sh
MINRELOAD=60
MAXRELOAD=900
MINCORRECTION=60
MAXCORRECTION=60
CORRECTIONRELOAD=60
HOSTNAME=sm50b
UPDATE_LOOPS=5

PNGREQUEST=png
PIDFILE="${SCRIPT_FILENAME}.pid"
DATAFILE="${SCRIPT_FILENAME}.dat"
BINARY="/mybin/sm50b-status/sm50b"

UPDATE_PID=$( cat "${PIDFILE}" 2>/dev/null ); [ "${UPDATE_PID}" ] || UPDATE_PID="none"
if ! kill -0 ${UPDATE_PID} 2>/dev/null; then
   ${BINARY} ${HOSTNAME} -b > "${DATAFILE}"
   {
      DELAY=$[ ${MINRELOAD} - 1 ]
      I=${UPDATE_LOOPS}
      while [ $I -gt 0 ]; do
        sleep ${DELAY}
        ${BINARY} ${HOSTNAME} -b > "${DATAFILE}"
        I=$[ $I - 1 ];
      done
   } 2>/dev/null >/dev/null </dev/null 2</dev/null &
   UPDATE_PID=$!
   disown 2>/dev/null
   echo "${UPDATE_PID}" > "${PIDFILE}"
fi

if [ "${QUERY_STRING}" != "${PNGREQUEST}" ]; then

  TIMEOUT=${QUERY_STRING}
  [ "$QUERY_STRING" -le "${MAXRELOAD}" ] || QUERY_STRING=${MAXCORRECTION} TIMEOUT=${CORRECTIONRELOAD}
  [ "$QUERY_STRING" -ge "${MINRELOAD}" ]   || QUERY_STRING=${MINCORRECTION} TIMEOUT=${CORRECTIONRELOAD}

  cat <<EOT
Content-Type: text/html

<html>
  <head>
    <meta http-equiv="refresh" content="${TIMEOUT}; URL=${SCRIPT_NAME}?${QUERY_STRING}">
    <title>Connection status of SpeedModem 50B on :  $(date -r "${DATAFILE}")</title>
  </head>
  <body>
    <pre>$( [ "$TIMEOUT" -ge "${MINRELOAD}" ] && ${BINARY} - < "${DATAFILE}" | grep -v 'MAC Address' )</pre><br>
    <center><img border=0 src="${SCRIPT_NAME}?${PNGREQUEST}"></center>
  </body>
</html>
EOT

else
echo -en 'Content-Type: image/png\n\n'
${BINARY} - -p < "${DATAFILE}"
fi

