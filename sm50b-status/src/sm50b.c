/***************************************************************************
 *   Copyright (C) 2005 by Ralf Miunske                                    *
 *   miunske at users.sourceforge.net                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details. (see COPYING)            *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   sm50b.c for linux                                                     *
 *   Tr*ndCh*p-status utility                                              *
 ***************************************************************************/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define REMOTE_SERVER_PORT 0xaaaa
#define MAX_MSG 1492
#define SOCKET_ERROR -1

#define ADR_BB  0x004
#define ADR_RAS 0x044
#define ADR_DSL 0x085
#define ADR_STD 0x0c4
#define ADR_COUNTRY 0x104
#define ADR_MAC 0x108
#define ADR_UPTIME 0x11c
#define ADR_LINESTATUS 0x144
#define ADR_BANDWIDTH_FAST_DOWN 0x14a
#define ADR_BANDWIDTH_INTER_DOWN 0x148
#define ADR_BANDWIDTH_FAST_UP 0x14e
#define ADR_BANDWIDTH_INTER_UP 0x14c
#define ADR_FEC_FAST_DOWN 0x154
#define ADR_FEC_INTER_DOWN 0x156
#define ADR_CRC_FAST_DOWN 0x158
#define ADR_CRC_INTER_DOWN 0x15a
#define ADR_HEC_FAST_DOWN 0x15c
#define ADR_HEC_INTER_DOWN 0x15e
#define ADR_FEC_FAST_UP 0x160
#define ADR_FEC_INTER_UP 0x162
#define ADR_CRC_FAST_UP 0x164
#define ADR_CRC_INTER_UP 0x166
#define ADR_HEC_FAST_UP 0x168
#define ADR_HEC_INTER_UP 0x16a
#define ADR_HOSTNAME 0x288
#define ADR_IP 0x2a8
#define ADR_MASK 0x2c8

#define VAL_LINESTATUS_DOWN 0x00
#define VAL_LINESTATUS_WFI  0x08
#define VAL_LINESTATUS_INIT 0x10
#define VAL_LINESTATUS_UP   0x20

#define TXT_BB "HW_FWVER_BOOTBASE"
#define TXT_RAS "HW_FWVER_RAS"
#define TXT_DSL "HW_FWVER_DSL"
#define TXT_STD "LINE_STANDARD"
#define TXT_COUNTRY "LINE_COUNTRY"
#define TXT_MAC "HW_MACADDR"
#define TXT_UPTIME "LINE_UPTIME"
#define TXT_LINESTATUS "LINE_STATUS"
#define TXT_LINESTATUS_DOWN "DOWN"
#define TXT_LINESTATUS_WFI "WAITING_FOR_INITIALIZATION"
#define TXT_LINESTATUS_INIT "INITIALIZING"
#define TXT_LINESTATUS_UP "UP"
#define TXT_LINESTATUS_UNKNOWN "unknown"
#define TXT_BANDWIDTH_FAST_DOWN "LINE_BW_DOWN_FASTPATH"
#define TXT_BANDWIDTH_INTER_DOWN "LINE_BW_DOWN_INTERLEAVED"
#define TXT_BANDWIDTH_FAST_UP "LINE_BW_UP_FASTPATH"
#define TXT_BANDWIDTH_INTER_UP "LINE_BW_UP_INTERLEAVED"
#define TXT_FEC_FAST_DOWN "LINE_ERR_FEC_DOWN_FASTPATH"
#define TXT_FEC_INTER_DOWN "LINE_ERR_FEC_DOWN_INTERLEAVED"
#define TXT_CRC_FAST_DOWN "LINE_ERR_CRC_DOWN_FASTPATH"
#define TXT_CRC_INTER_DOWN "LINE_ERR_CRC_DOWN_INTERLEAVED"
#define TXT_HEC_FAST_DOWN "LINE_ERR_HEC_DOWN_FASTPATH"
#define TXT_HEC_INTER_DOWN "LINE_ERR_HEC_DOWN_INTERLEAVED"
#define TXT_FEC_FAST_UP "LINE_ERR_FEC_UP_FASTPATH"
#define TXT_FEC_INTER_UP "LINE_ERR_FEC_UP_INTERLEAVED"
#define TXT_CRC_FAST_UP "LINE_ERR_CRC_UP_FASTPATH"
#define TXT_CRC_INTER_UP "LINE_ERR_CRC_UP_INTERLEAVED"
#define TXT_HEC_FAST_UP "LINE_ERR_HEC_UP_FASTPATH"
#define TXT_HEC_INTER_UP "LINE_ERR_HEC_UP_INTERLEAVED"
#define TXT_HOSTNAME "HOSTNAME"
#define TXT_IP "LAN_IP"
#define TXT_MASK "LAN_NETMASK"

int isReadable(int sd,int * error,int timeOut) {
  fd_set socketReadSet;
  FD_ZERO(&socketReadSet);
  FD_SET(sd,&socketReadSet);
  struct timeval tv;
  if (timeOut) {
    tv.tv_sec  = timeOut / 1000;
    tv.tv_usec = (timeOut % 1000) * 1000;
  } else {
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
  } // if
  if (select(sd+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
    *error = 1;
    return 0;
  } // if
  *error = 0;
  return FD_ISSET(sd,&socketReadSet) != 0;
} /* isReadable */

char buffer[2];

void byteSwap(char* src, int* dst) {
   (*dst)=256*(src[0]<0?256+src[0]:src[0])+(src[1]<0?256+src[1]:src[1]);
}

void printLineStatus(int status) {
   switch(status) {
      case VAL_LINESTATUS_DOWN:
          printf(TXT_LINESTATUS_DOWN);
          break;
      
      case VAL_LINESTATUS_WFI:
          printf(TXT_LINESTATUS_WFI);
          break;
      
      case VAL_LINESTATUS_INIT:
          printf(TXT_LINESTATUS_INIT);
          break;
      
      case VAL_LINESTATUS_UP:
          printf(TXT_LINESTATUS_UP);
          break;

      default:
          printf(TXT_LINESTATUS_UNKNOWN);
          break;
   }
}

int main(int argc, char *argv[]) {
  
  unsigned short int s;
  int sd, rc, i, n, echoLen, flags, error, timeOut, retries;
  struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
  struct hostent *h;
  char msg[MAX_MSG];
  char* data_BB=&msg[ADR_BB];
  char* data_RAS=&msg[ADR_RAS];
  char* data_DSL=&msg[ADR_DSL];
  char* data_STD=&msg[ADR_STD];
  char* data_COUNTRY=&msg[ADR_COUNTRY];
  char* data_MAC=&msg[ADR_MAC];
  char* data_UPTIME=&msg[ADR_UPTIME];
  char* data_HOSTNAME=&msg[ADR_HOSTNAME];
  char* data_IP=&msg[ADR_IP];
  char* data_MASK=&msg[ADR_MASK];

  char* data_BANDWIDTH_FAST_DOWN_buf=&msg[ADR_BANDWIDTH_FAST_DOWN];
  char* data_BANDWIDTH_INTER_DOWN_buf=&msg[ADR_BANDWIDTH_INTER_DOWN];
  char* data_BANDWIDTH_FAST_UP_buf=&msg[ADR_BANDWIDTH_FAST_UP];
  char* data_BANDWIDTH_INTER_UP_buf=&msg[ADR_BANDWIDTH_INTER_UP];
  char* data_FEC_FAST_DOWN_buf=&msg[ADR_FEC_FAST_DOWN];
  char* data_FEC_INTER_DOWN_buf=&msg[ADR_FEC_INTER_DOWN];
  char* data_CRC_FAST_DOWN_buf=&msg[ADR_CRC_FAST_DOWN];
  char* data_CRC_INTER_DOWN_buf=&msg[ADR_CRC_INTER_DOWN];
  char* data_HEC_FAST_DOWN_buf=&msg[ADR_HEC_FAST_DOWN];
  char* data_HEC_INTER_DOWN_buf=&msg[ADR_HEC_INTER_DOWN];
  char* data_FEC_FAST_UP_buf=&msg[ADR_FEC_FAST_UP];
  char* data_FEC_INTER_UP_buf=&msg[ADR_FEC_INTER_UP];
  char* data_CRC_FAST_UP_buf=&msg[ADR_CRC_FAST_UP];
  char* data_CRC_INTER_UP_buf=&msg[ADR_CRC_INTER_UP];
  char* data_HEC_FAST_UP_buf=&msg[ADR_HEC_FAST_UP];
  char* data_HEC_INTER_UP_buf=&msg[ADR_HEC_INTER_UP];

  unsigned int data_LINESTATUS;
  unsigned int data_BANDWIDTH_FAST_DOWN;
  unsigned int data_BANDWIDTH_INTER_DOWN;
  unsigned int data_BANDWIDTH_FAST_UP;
  unsigned int data_BANDWIDTH_INTER_UP;
  unsigned int data_FEC_FAST_DOWN;
  unsigned int data_FEC_INTER_DOWN;
  unsigned int data_CRC_FAST_DOWN;
  unsigned int data_CRC_INTER_DOWN;
  unsigned int data_HEC_FAST_DOWN;
  unsigned int data_HEC_INTER_DOWN;
  unsigned int data_FEC_FAST_UP;
  unsigned int data_FEC_INTER_UP;
  unsigned int data_CRC_FAST_UP;
  unsigned int data_CRC_INTER_UP;
  unsigned int data_HEC_FAST_UP;
  unsigned int data_HEC_INTER_UP;

  if(argc!=2 && argc!=3) {
    printf("usage : %s <server> [-(h|s|b)]\n", argv[0]);
    printf("with  : -h human readable (default)\n");
    printf("        -s script readable\n");
    printf("        -b binary\n");
    exit(1);
  }

  h = gethostbyname(argv[1]);
  if(h==NULL) { printf("%s: unknown host '%s' \n", argv[0], argv[1]); exit(1); }

  remoteServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) { printf("%s: cannot open socket \n",argv[0]); exit(1); }
  
  cliAddr.sin_family = AF_INET; cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  cliAddr.sin_port = htons(0);

  rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  if(rc<0) { printf("%s: cannot bind port\n", argv[0]); exit(1); }

  flags = 0;

  timeOut = 100; // ms
  retries = 5; // times

  buffer[0]=0; buffer[1]=1;
  rc = sendto(sd, buffer, 2, flags, (struct sockaddr *)(&remoteServAddr), sizeof(remoteServAddr));

  if(rc<0) { printf("%s: cannot send data %d \n",argv[0],i-1); close(sd); exit(1); }

    memset(msg,0x0,MAX_MSG);

    while (retries>=0 && !isReadable(sd,&error,timeOut)) --retries;
    if(retries<0) { printf("%s: no answer\n",argv[0]); close(sd); exit(1); }

    echoLen = sizeof(echoServAddr);
    n = recvfrom(sd, msg, MAX_MSG, flags, (struct sockaddr*)(&echoServAddr), &echoLen);
    if(n<0) { printf("%s: cannot receive data \n",argv[0]); close(sd); exit(1); }

    byteSwap(data_BANDWIDTH_FAST_DOWN_buf, &data_BANDWIDTH_FAST_DOWN); byteSwap(data_BANDWIDTH_INTER_DOWN_buf, &data_BANDWIDTH_INTER_DOWN);
    byteSwap(data_BANDWIDTH_FAST_UP_buf, &data_BANDWIDTH_FAST_UP); byteSwap(data_BANDWIDTH_INTER_UP_buf, &data_BANDWIDTH_INTER_UP);
    byteSwap(data_FEC_FAST_DOWN_buf, &data_FEC_FAST_DOWN); byteSwap(data_FEC_INTER_DOWN_buf, &data_FEC_INTER_DOWN);
    byteSwap(data_CRC_FAST_DOWN_buf, &data_CRC_FAST_DOWN); byteSwap(data_CRC_INTER_DOWN_buf, &data_CRC_INTER_DOWN);
    byteSwap(data_HEC_FAST_DOWN_buf, &data_HEC_FAST_DOWN); byteSwap(data_HEC_INTER_DOWN_buf, &data_HEC_INTER_DOWN);
    byteSwap(data_FEC_FAST_UP_buf, &data_FEC_FAST_UP); byteSwap(data_FEC_INTER_UP_buf, &data_FEC_INTER_UP);
    byteSwap(data_CRC_FAST_UP_buf, &data_CRC_FAST_UP); byteSwap(data_CRC_INTER_UP_buf, &data_CRC_INTER_UP);
    byteSwap(data_HEC_FAST_UP_buf, &data_HEC_FAST_UP); byteSwap(data_HEC_INTER_UP_buf, &data_HEC_INTER_UP);
    data_LINESTATUS=msg[ADR_LINESTATUS];

    buffer[0]=(argc>2&&strlen(argv[2])==2)?argv[2][1]:'h';
    switch(buffer[0]) {
       case 's':
          printf("%s=%s\n", TXT_BB, data_BB);
          printf("%s=%s\n", TXT_RAS, data_RAS);
          printf("%s=%s\n", TXT_DSL, data_DSL);
          printf("%s=%s\n", TXT_MAC, data_MAC);
          printf("%s=%s\n", TXT_STD, data_STD);
          printf("%s=%s\n", TXT_COUNTRY, data_COUNTRY);
          printf("%s=",     TXT_LINESTATUS); printLineStatus(data_LINESTATUS); printf("\n");
          printf("%s=%s\n", TXT_UPTIME, data_UPTIME);
          printf("%s=%u\n", TXT_BANDWIDTH_FAST_DOWN, data_BANDWIDTH_FAST_DOWN);
          printf("%s=%u\n", TXT_BANDWIDTH_INTER_DOWN, data_BANDWIDTH_INTER_DOWN);
          printf("%s=%u\n", TXT_BANDWIDTH_FAST_UP, data_BANDWIDTH_FAST_UP);
          printf("%s=%u\n", TXT_BANDWIDTH_INTER_UP, data_BANDWIDTH_INTER_UP);
          printf("%s=%u\n", TXT_FEC_FAST_DOWN, data_FEC_FAST_DOWN);
          printf("%s=%u\n", TXT_FEC_INTER_DOWN, data_FEC_INTER_DOWN);
          printf("%s=%u\n", TXT_CRC_FAST_DOWN, data_CRC_FAST_DOWN);
          printf("%s=%u\n", TXT_CRC_INTER_DOWN, data_CRC_INTER_DOWN);
          printf("%s=%u\n", TXT_HEC_FAST_DOWN, data_HEC_FAST_DOWN);
          printf("%s=%u\n", TXT_HEC_INTER_DOWN, data_HEC_INTER_DOWN);
          printf("%s=%u\n", TXT_FEC_FAST_UP, data_FEC_FAST_UP);
          printf("%s=%u\n", TXT_FEC_INTER_UP, data_FEC_INTER_UP);
          printf("%s=%u\n", TXT_CRC_FAST_UP, data_CRC_FAST_UP);
          printf("%s=%u\n", TXT_CRC_INTER_UP, data_CRC_INTER_UP);
          printf("%s=%u\n", TXT_HEC_FAST_UP, data_HEC_FAST_UP);
          printf("%s=%u\n", TXT_HEC_INTER_UP, data_HEC_INTER_UP);
          printf("%s=%s\n", TXT_HOSTNAME, data_HOSTNAME);
          printf("%s=%s\n", TXT_IP, data_IP);
          printf("%s=%s\n", TXT_MASK, data_MASK);
          break;

       case 'b':
          for(i=0; i<n; ++i) printf("%c", msg[i]); // RAW
          break;

       case 'h':
          printf("Firmware Information:\n");
          printf("Bootbase Version : %s\n",   data_BB);
          printf("RAS F/W Version  : %s\n",   data_RAS);
          printf("DSL FW Version   : %s\n",   data_DSL);
          printf("Standard         : %s\n",   data_STD);
          printf("Country Code     : %s\n",   data_COUNTRY);
          printf("MAC Address      : %s\n",   data_MAC);
          printf("Hostname         : %s\n\n", data_HOSTNAME);

          printf("ADSL Status:\n");
          printf("ADSL Status : "); printLineStatus(data_LINESTATUS); printf("\n");
          printf("ADSL Uptime : %s\n", data_UPTIME);
          printf("                                down         up\n");
          printf("Bit-rate  (fast)        : %10u %10u\n", data_BANDWIDTH_FAST_DOWN, data_BANDWIDTH_FAST_UP);
          printf("Bit-rate  (interleaved) : %10u %10u\n", data_BANDWIDTH_INTER_DOWN, data_BANDWIDTH_INTER_UP);
          printf("FEC error (fast)        : %10u %10u\n", data_FEC_FAST_DOWN, data_FEC_FAST_UP);
          printf("FEC error (interleaved) : %10u %10u\n", data_FEC_INTER_DOWN, data_FEC_INTER_UP);
          printf("CRC error (fast)        : %10u %10u\n", data_CRC_FAST_DOWN, data_CRC_FAST_UP);
          printf("CRC error (interleaved) : %10u %10u\n", data_CRC_INTER_DOWN, data_CRC_INTER_UP);
          printf("HEC error (fast)        : %10u %10u\n", data_HEC_FAST_DOWN, data_HEC_FAST_UP);
          printf("HEC error (interleaved) : %10u %10u\n", data_HEC_INTER_DOWN, data_HEC_INTER_UP);
          break;

       default:
          printf("%s: unknown display method '%c'\n",argv[0], buffer[0]);
          return 1;
          break;
    }

  return 0;
}
