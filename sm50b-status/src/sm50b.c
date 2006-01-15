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

#ifdef HAVE_LIBPNG
#include <png.h>
#endif

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

// DOWN = near end  UP = far end
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

#define ADR_LINE_RELLOAD_DOWN 0x16c   // in %
#define ADR_LINE_NOISE_DOWN 0x16e     // in 1/10*dB
#define ADR_LINE_XMITPWR_UP 0x170     // in 1/10*dBm
#define ADR_LINE_ATT_DOWN 0x172       // in 1/10*dB
#define ADR_LINE_RELLOAD_UP 0x174     // in %
#define ADR_LINE_NOISE_UP 0x176       // in 1/10*dB
#define ADR_LINE_XMITPWR_DOWN 0x178   // in 1/10*dBm
#define ADR_LINE_ATT_UP 0x17a         // in 1/10*dB

#define ADR_TONE0 0x17c
#define ADR_TONE_END 0x27c
#define ADR_HOSTNAME 0x288
#define ADR_IP 0x2a8
#define ADR_MASK 0x2c8

#define ADR_VC_QOS 0x2e8
#define ADR_VC_VPI 0x2ea
#define ADR_VC_VCI 0x2ec

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

#define TXT_LINE_RELLOAD_DOWN "LINE_RELLOAD_DOWN"
#define TXT_LINE_NOISE_DOWN "LINE_NOISE_DOWN"
#define TXT_LINE_XMITPWR_UP "LINE_XMITPWR_UP"
#define TXT_LINE_ATT_DOWN "LINE_ATT_DOWN"
#define TXT_LINE_RELLOAD_UP "LINE_RELLOAD_UP"
#define TXT_LINE_NOISE_UP "LINE_NOISE_UP"
#define TXT_LINE_XMITPWR_DOWN "LINE_XMITPWR_DOWN"
#define TXT_LINE_ATT_UP "LINE_ATT_UP"

#define TXT_TONE "TONES"
#define TXT_HOSTNAME "HOSTNAME"
#define TXT_IP "LAN_IP"
#define TXT_MASK "LAN_NETMASK"

#define TXT_VC_QOS "VC_QOS"
#define TXT_VC_VPI "VC_VPI"
#define TXT_VC_VCI "VC_VCI"

#define TXT_FIRSTCHANNEL_UP "FIRSTCHANNEL_UP"
#define TXT_LASTCHANNEL_UP "LASTCHANNEL_UP"
#define TXT_FIRSTCHANNEL_DOWN "FIRSTCHANNEL_DOWN"
#define TXT_LASTCHANNEL_DOWN "LASTCHANNEL_DOWN"
#define TXT_GAPS "GAPS"

#define def_firstDownstream 61
#define def_pilotTone 96

#ifdef HAVE_LIBPNG
#define png_col_R 0
#define png_col_G 1
#define png_col_B 2
#define png_col_A 3

#define def_diag_height 96
#define def_diag_width_wide 1024
#define def_diag_width_narrow 512
#define def_diag_margin 20
#define def_mark_tone_len 4
#define def_mark_bit_len 4
#define def_mark_bit_longlen 6

#define bgR 233
#define bgG 241
#define bgB 254
#define bgA 255
#define diagR 138
#define diagG 179
#define diagB 189
#define diagA 255
#define bitR 128
#define bitG 128
#define bitB 128
#define bitA 255
#define markR 0
#define markG 0
#define markB 0
#define markA 255
#define upstreamR 0
#define upstreamG 255 
#define upstreamB 0
#define upstreamA 255
#define downstreamR 0
#define downstreamG 0
#define downstreamB 255
#define downstreamA 255
#define pilotR 240
#define pilotG 30
#define pilotB 30
#define pilotA 255
#define zeroR 220
#define zeroG 220
#define zeroB 30
#define zeroA 255
#define def_diag_tones 256
#define def_diag_bits 16
#define def_mark_nth_tone 32
#endif

#define def_diag_fasttones 512

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

char msg[MAX_MSG];
void displayAt(unsigned int adr) {
   unsigned int data;
   char* buf = &msg[adr];
   byteSwap(buf, &data);
   printf("0x%04x : %5u\n", adr, data);
}

unsigned char* data_TONE=&msg[ADR_TONE0];
unsigned int getTone(int tone) {
   if(tone<0) return 0;
   if(tone>=def_diag_fasttones) return 0;
   return (tone%2)?data_TONE[tone/2]/16:data_TONE[tone/2]%16;
}

int main(int argc, char *argv[]) {
  unsigned short int s;
  int sd, rc, i, n, echoLen, flags, error, timeOut, retries, tone;
  struct sockaddr_in cliAddr, remoteServAddr, echoServAddr;
  struct hostent *h;
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

  char* data_LINE_RELLOAD_DOWN_buf=&msg[ADR_LINE_RELLOAD_DOWN];
  char* data_LINE_NOISE_DOWN_buf=&msg[ADR_LINE_NOISE_DOWN];
  char* data_LINE_XMITPWR_UP_buf=&msg[ADR_LINE_XMITPWR_UP];
  char* data_LINE_ATT_DOWN_buf=&msg[ADR_LINE_ATT_DOWN];
  char* data_LINE_RELLOAD_UP_buf=&msg[ADR_LINE_RELLOAD_UP];
  char* data_LINE_NOISE_UP_buf=&msg[ADR_LINE_NOISE_UP];
  char* data_LINE_XMITPWR_DOWN_buf=&msg[ADR_LINE_XMITPWR_DOWN];
  char* data_LINE_ATT_UP_buf=&msg[ADR_LINE_ATT_UP];
  char* data_VC_QOS_buf=&msg[ADR_VC_QOS];
  char* data_VC_VPI_buf=&msg[ADR_VC_VPI];
  char* data_VC_VCI_buf=&msg[ADR_VC_VCI];

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
  unsigned int data_LINE_RELLOAD_DOWN;
  unsigned int data_LINE_NOISE_DOWN;
  unsigned int data_LINE_XMITPWR_UP;
  unsigned int data_LINE_ATT_DOWN;
  unsigned int data_LINE_RELLOAD_UP;
  unsigned int data_LINE_NOISE_UP;
  unsigned int data_LINE_XMITPWR_DOWN;
  unsigned int data_LINE_ATT_UP;
  double frac_LINE_NOISE_DOWN;
  double frac_LINE_XMITPWR_UP;
  double frac_LINE_ATT_DOWN;
  double frac_LINE_NOISE_UP;
  double frac_LINE_XMITPWR_DOWN;
  double frac_LINE_ATT_UP;
  unsigned int data_VC_QOS;
  unsigned int data_VC_VPI;
  unsigned int data_VC_VCI;

  unsigned int data_FIRSTCHANNEL_UP;
  unsigned int data_LASTCHANNEL_UP;
  unsigned int data_FIRSTCHANNEL_DOWN=def_firstDownstream;
  unsigned int data_LASTCHANNEL_DOWN;
  unsigned int data_GAPS[def_diag_fasttones];
  unsigned int data_BITSUM_UP;
  unsigned int data_BITSUM_DOWN;
  unsigned float data_BITQUOTE_UP;
  unsigned float data_BITQUOTE_DOWN;

  unsigned int pilotTone=def_pilotTone;
#ifdef HAVE_LIBPNG
  // libPNG-stuff
  int x, y, bits, bits_prev, bits_next;

  int width, height;
  png_byte color_type;
  png_byte bit_depth;

  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * rows;
  png_byte* row;
  png_byte* pixel;

  unsigned int diag_height=def_diag_height;
  unsigned int diag_width=def_diag_width_narrow;
  unsigned int diag_margin=def_diag_margin;
  unsigned int mark_tone_len=def_mark_tone_len;
  unsigned int mark_bit_len=def_mark_bit_len;
  unsigned int mark_bit_longlen=def_mark_bit_longlen;
  unsigned int diag_tones=def_diag_tones;
  unsigned int diag_bits=def_diag_bits;
  unsigned int mark_nth_tone=def_mark_nth_tone;
#endif

  if(argc!=2 && argc!=3) {
    printf("usage : %s (<server>|-) [-(h|s|b", argv[0]);
#ifdef HAVE_LIBPNG
    printf("|p");
#endif
    printf(")]\n");
    printf("with  : -h human readable (default)\n");
    printf("        -s script readable\n");
    printf("        -b binary\n");
#ifdef HAVE_LIBPNG
    printf("        -p PNG image of tones\n");
#endif
    printf("        <server> CELL-19A-BX-hostname or\n");
    printf("        -        for stdin\n");
    exit(1);
  }
  if(strlen(argv[1])==1 && argv[1][0]=='-') {
    // stdin-mode
    memset(msg,0x0,MAX_MSG);
    if(fread(msg, 1, MAX_MSG, stdin)<756) { printf("%s: cannot read valid data from stdin\n", argv[0]); exit(1); }
  } else {
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
  }

    byteSwap(data_BANDWIDTH_FAST_DOWN_buf, &data_BANDWIDTH_FAST_DOWN); byteSwap(data_BANDWIDTH_INTER_DOWN_buf, &data_BANDWIDTH_INTER_DOWN);
    byteSwap(data_BANDWIDTH_FAST_UP_buf, &data_BANDWIDTH_FAST_UP); byteSwap(data_BANDWIDTH_INTER_UP_buf, &data_BANDWIDTH_INTER_UP);
    byteSwap(data_FEC_FAST_DOWN_buf, &data_FEC_FAST_DOWN); byteSwap(data_FEC_INTER_DOWN_buf, &data_FEC_INTER_DOWN);
    byteSwap(data_CRC_FAST_DOWN_buf, &data_CRC_FAST_DOWN); byteSwap(data_CRC_INTER_DOWN_buf, &data_CRC_INTER_DOWN);
    byteSwap(data_HEC_FAST_DOWN_buf, &data_HEC_FAST_DOWN); byteSwap(data_HEC_INTER_DOWN_buf, &data_HEC_INTER_DOWN);
    byteSwap(data_FEC_FAST_UP_buf, &data_FEC_FAST_UP); byteSwap(data_FEC_INTER_UP_buf, &data_FEC_INTER_UP);
    byteSwap(data_CRC_FAST_UP_buf, &data_CRC_FAST_UP); byteSwap(data_CRC_INTER_UP_buf, &data_CRC_INTER_UP);
    byteSwap(data_HEC_FAST_UP_buf, &data_HEC_FAST_UP); byteSwap(data_HEC_INTER_UP_buf, &data_HEC_INTER_UP);
    byteSwap(data_LINE_RELLOAD_DOWN_buf, &data_LINE_RELLOAD_DOWN); byteSwap(data_LINE_NOISE_DOWN_buf, &data_LINE_NOISE_DOWN);
    byteSwap(data_LINE_XMITPWR_UP_buf, &data_LINE_XMITPWR_UP); byteSwap(data_LINE_ATT_DOWN_buf, &data_LINE_ATT_DOWN);
    byteSwap(data_LINE_RELLOAD_UP_buf, &data_LINE_RELLOAD_UP); byteSwap(data_LINE_NOISE_UP_buf, &data_LINE_NOISE_UP);
    byteSwap(data_LINE_XMITPWR_DOWN_buf, &data_LINE_XMITPWR_DOWN); byteSwap(data_LINE_ATT_UP_buf, &data_LINE_ATT_UP);
    byteSwap(data_VC_QOS_buf, &data_VC_QOS); byteSwap(data_VC_VPI_buf, &data_VC_VPI); byteSwap(data_VC_VCI_buf, &data_VC_VCI);

    frac_LINE_NOISE_DOWN=0.1f*((double)data_LINE_NOISE_DOWN);
    frac_LINE_XMITPWR_UP=0.1f*((double)data_LINE_XMITPWR_UP);
    frac_LINE_ATT_DOWN=0.1f*((double)data_LINE_ATT_DOWN);
    frac_LINE_NOISE_UP=0.1f*((double)data_LINE_NOISE_UP);
    frac_LINE_XMITPWR_DOWN=0.1f*((double)data_LINE_XMITPWR_DOWN);
    frac_LINE_ATT_UP=0.1f*((double)data_LINE_ATT_UP);

    data_LINESTATUS=msg[ADR_LINESTATUS];

    for(tone=def_diag_fasttones; (tone>0)&&(getTone(tone)==0); --tone); data_LASTCHANNEL_DOWN=tone;
    for(tone=0; (tone<def_diag_fasttones)&&(getTone(tone)==0); ++tone); data_FIRSTCHANNEL_UP=tone;

    for(tone=data_FIRSTCHANNEL_UP;(tone<def_diag_fasttones)&&(getTone(tone)!=0||getTone(tone+1)!=0||getTone(tone+1)!=0); ++tone); data_LASTCHANNEL_UP=tone-1;
    if(data_LASTCHANNEL_UP+1>=pilotTone)
       for(tone=data_FIRSTCHANNEL_UP;(tone<def_diag_fasttones)&&(getTone(tone)!=0||getTone(tone+1)!=0); ++tone); data_LASTCHANNEL_UP=tone-1;
    if(data_LASTCHANNEL_UP+1>=pilotTone)
       for(tone=data_FIRSTCHANNEL_UP;(tone<def_diag_fasttones)&&(getTone(tone)!=0); ++tone); data_LASTCHANNEL_UP=tone-1;
    if(data_LASTCHANNEL_UP+1>=pilotTone) data_LASTCHANNEL_UP=def_firstDownstream-1;

    for(tone=data_LASTCHANNEL_UP+1; (tone<def_diag_fasttones)&&(getTone(tone)==0); ++tone); data_FIRSTCHANNEL_DOWN=tone;
    data_FIRSTCHANNEL_DOWN=(tone<(data_LASTCHANNEL_DOWN/2))?tone:def_firstDownstream;

    data_BITSUM_UP=data_BITSUM_DOWN=0;
    for(tone=data_FIRSTCHANNEL_UP; tone<=data_LASTCHANNEL_DOWN; ++tone) {
       data_GAPS[tone]=0;
       if(tone<=data_LASTCHANNEL_UP||tone>=data_FIRSTCHANNEL_DOWN) {
          if((getTone(tone-1)!=0)&&(getTone(tone)==0)) {
             for(i=tone; (i<=data_LASTCHANNEL_DOWN)&&(getTone(i)==0); ++i); --i;
             data_GAPS[(tone+i)/2]=1;
             tone=i;
          }
          if(tone<data_FIRSTCHANNEL_DOWN) data_BITSUM_UP+=getTone(tone); else data_BITSUM_DOWN+=getTone(tone);
       }
    }
   data_BITQUOTE_UP   = ((float)(data_BANDWIDTH_FAST_DOWN+data_BANDWIDTH_INTER_DOWN))/((float)data_BITSUM_DOWN);
   data_BITQUOTE_DOWN = ((float)(data_BANDWIDTH_FAST_UP+data_BANDWIDTH_INTER_UP))/((float)data_BITSUM_UP);

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
          printf("%s=%u\n", TXT_LINE_RELLOAD_DOWN, data_LINE_RELLOAD_DOWN);
          printf("%s=%.1f\n", TXT_LINE_NOISE_DOWN, frac_LINE_NOISE_DOWN);
          printf("%s=%.1f\n", TXT_LINE_XMITPWR_UP, frac_LINE_XMITPWR_UP);
          printf("%s=%.1f\n", TXT_LINE_ATT_DOWN, frac_LINE_ATT_DOWN);
          printf("%s=%u\n", TXT_LINE_RELLOAD_UP, data_LINE_RELLOAD_UP);
          printf("%s=%.1f\n", TXT_LINE_NOISE_UP, frac_LINE_NOISE_UP);
          printf("%s=%.1f\n", TXT_LINE_XMITPWR_DOWN, frac_LINE_XMITPWR_DOWN);
          printf("%s=%.1f\n", TXT_LINE_ATT_UP, frac_LINE_ATT_UP);

          printf("%s=%u,%u", TXT_TONE, data_TONE[0]/16, data_TONE[0]%16);
             for(tone=1; tone<(ADR_TONE_END-ADR_TONE0); ++tone) {
                printf(",%u,%u", data_TONE[tone]/16, data_TONE[tone]%16 );
             }
          printf("\n");
          printf("%s=%s\n", TXT_HOSTNAME, data_HOSTNAME);
          printf("%s=%s\n", TXT_IP, data_IP);
          printf("%s=%s\n", TXT_MASK, data_MASK);
          printf("%s=%u\n", TXT_VC_QOS, data_VC_QOS);
          printf("%s=%u\n", TXT_VC_VPI, data_VC_VPI);
          printf("%s=%u\n", TXT_VC_VCI, data_VC_VCI);

          printf("%s=%u\n", TXT_FIRSTCHANNEL_UP, data_FIRSTCHANNEL_UP);
          printf("%s=%u\n", TXT_LASTCHANNEL_UP, data_LASTCHANNEL_UP);
          printf("%s=%u\n", TXT_FIRSTCHANNEL_DOWN, data_FIRSTCHANNEL_DOWN);
          printf("%s=%u\n", TXT_LASTCHANNEL_DOWN, data_LASTCHANNEL_DOWN);
          printf("%s=", TXT_GAPS);
          for(tone=data_FIRSTCHANNEL_UP; tone<=data_LASTCHANNEL_DOWN && data_GAPS[tone]==0; ++tone);
          if(tone<=data_LASTCHANNEL_DOWN && data_GAPS[tone]) printf("%u", tone);
          for(++tone; tone<=data_LASTCHANNEL_DOWN; ++tone)
            if(data_GAPS[tone]) printf(",%u", tone);
          printf("\n");
          break;

       case 'b':
          for(i=0; i<n; ++i) printf("%c", msg[i]); // RAW
          break;

#ifdef HAVE_LIBPNG
       case 'p':
          diag_tones=(data_LASTCHANNEL_DOWN>=def_diag_tones)?def_diag_fasttones:def_diag_tones;
          diag_width=2*diag_tones;


          height=diag_height+(2*diag_margin);
          width=diag_width+(2*diag_margin);
          
          bit_depth=8;
          rows=malloc(height*sizeof(png_bytep));
          for (y=0; y<height; y++)
		rows[y]=malloc(width*bit_depth*4);

          for (x=0; x<width; x++) {
            for (y=0; y<height; y++) {
              row = rows[y]; pixel=&(row[x*4]);

              pixel[png_col_R]=bgR; pixel[png_col_G]=bgG; pixel[png_col_B]=bgB; pixel[png_col_A]=bgA;

              if(x+1==diag_margin && (y>=diag_margin && y<=height-diag_margin) ) { pixel[png_col_R]=markR; pixel[png_col_G]=markG; pixel[png_col_B]=markB; pixel[png_col_A]=markA; }

              if((x>(diag_margin-mark_bit_longlen)) && (x<diag_margin) && y>=diag_margin && y<=height-diag_margin && !((height-y-diag_margin)%(diag_height/diag_bits))) {
                if(x>(diag_margin-mark_bit_len)) { pixel[png_col_R]=markR; pixel[png_col_G]=markG; pixel[png_col_B]=markB; pixel[png_col_A]=markA; }
                if(!((height-y-diag_margin)%(diag_height/8))) { pixel[png_col_R]=markR; pixel[png_col_G]=markG; pixel[png_col_B]=markB; pixel[png_col_A]=markA; }
              }

              if(x>=diag_margin && x<width-diag_margin) {
                tone=(x-diag_margin)*diag_tones/diag_width;
                if(y==height-diag_margin) { pixel[png_col_R]=markR; pixel[png_col_G]=markG; pixel[png_col_B]=markB; pixel[png_col_A]=markA; }
                if( (y>=height-diag_margin+1) &&
                    (y<=height-diag_margin+mark_tone_len) &&
                    !(tone%mark_nth_tone)) { pixel[png_col_R]=markR; pixel[png_col_G]=markG; pixel[png_col_B]=markB; pixel[png_col_A]=markA; }

                if(y>=diag_margin && y<height-diag_margin) {
                  bits=getTone(tone);

                  if(height-y-diag_margin>bits*(diag_height/diag_bits)) {
                   // diag - hintergrund
                   if(tone==pilotTone) { 
                     pixel[png_col_R]=pilotR; pixel[png_col_G]=pilotG; pixel[png_col_B]=pilotB; pixel[png_col_A]=pilotA;
                   } else if(! ((height-y-diag_margin)%(diag_height/8)) ) {
                     pixel[png_col_R]=bitR; pixel[png_col_G]=bitG; pixel[png_col_B]=bitB; pixel[png_col_A]=bitA;
                   } else {
                     pixel[png_col_R]=diagR; pixel[png_col_G]=diagG; pixel[png_col_B]=diagB; pixel[png_col_A]=diagA;
                   }
                   if(bits==0 && tone!=pilotTone) {
                      bits_next=(tone+1)<diag_tones?getTone(tone+1):0;
                      bits_prev=(tone-1)>=0?getTone(tone-1):0;
                      if((bits_prev!=0) || (bits_next!=0)) {
                        pixel[png_col_R]=zeroR; pixel[png_col_G]=zeroG; pixel[png_col_B]=zeroB; pixel[png_col_A]=zeroA;
                      }
                   }
                  } else {
                    // balken
                    if(tone>=data_FIRSTCHANNEL_DOWN) {
                      pixel[png_col_R]=downstreamR; pixel[png_col_G]=downstreamG; pixel[png_col_B]=downstreamB; pixel[png_col_A]=downstreamA;
                    } else {
                      pixel[png_col_R]=upstreamR; pixel[png_col_G]=upstreamG; pixel[png_col_B]=upstreamB; pixel[png_col_A]=upstreamA;
                    }
                  }
                }
              }
            }
          }

          /* initialize stuff */
	  if(!(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))) { printf("error 1\n"); return 1; }
	  if(!(info_ptr = png_create_info_struct(png_ptr))) { printf("error 2"); return 1; }
          if(setjmp(png_jmpbuf(png_ptr))) { printf("error 3"); return 1; }
          png_init_io(png_ptr, stdout);

          /* write header */
          if(setjmp(png_jmpbuf(png_ptr))) { printf("error 4"); return 1; }
          png_set_IHDR(png_ptr, info_ptr, width, height,
		     bit_depth, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	  png_write_info(png_ptr, info_ptr);

          /* write bytes */
          if(setjmp(png_jmpbuf(png_ptr))) { printf("error 5"); return 1; }
          png_write_image(png_ptr, rows);

          /* end write */
          if(setjmp(png_jmpbuf(png_ptr))) { printf("error 6"); return 1; }
          png_write_end(png_ptr, NULL);

          for (y=0; y<height; y++)
		free(rows[y]);
	  free(rows);
          break;
#endif

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
          printf("LINE Status : "); printLineStatus(data_LINESTATUS); printf("\n");
          printf("LINE Uptime : %s\n", data_UPTIME);
          printf("ATM vc      : vpi=%u vci=%u qos=%u\n", data_VC_VPI, data_VC_VCI, data_VC_QOS);
          printf("                                down         up\n");
          printf("Bit-rate  (fast)          : %10u %10u\n", data_BANDWIDTH_FAST_DOWN, data_BANDWIDTH_FAST_UP);
          printf("Bit-rate  (interleaved)   : %10u %10u\n", data_BANDWIDTH_INTER_DOWN, data_BANDWIDTH_INTER_UP);
          printf("Bit-rate  (relative cap.) : %8u %% %8u %%\n", data_LINE_RELLOAD_DOWN, data_LINE_RELLOAD_UP);
          printf("FEC error (fast)          : %10u %10u\n", data_FEC_FAST_DOWN, data_FEC_FAST_UP);
          printf("FEC error (interleaved)   : %10u %10u\n", data_FEC_INTER_DOWN, data_FEC_INTER_UP);
          printf("CRC error (fast)          : %10u %10u\n", data_CRC_FAST_DOWN, data_CRC_FAST_UP);
          printf("CRC error (interleaved)   : %10u %10u\n", data_CRC_INTER_DOWN, data_CRC_INTER_UP);
          printf("HEC error (fast)          : %10u %10u\n", data_HEC_FAST_DOWN, data_HEC_FAST_UP);
          printf("HEC error (interleaved)   : %10u %10u\n", data_HEC_INTER_DOWN, data_HEC_INTER_UP);
          printf("Noise margin              :  %6.1f dB  %6.1f dB \n", frac_LINE_NOISE_DOWN, frac_LINE_NOISE_UP);
          printf("Attenuation               :  %6.1f dB  %6.1f dB \n", frac_LINE_ATT_DOWN, frac_LINE_ATT_UP);
          printf("Transmit power            :  %6.1f dBm %6.1f dBm\n", frac_LINE_XMITPWR_DOWN, frac_LINE_XMITPWR_UP);
          printf("First channel             : %10u %10u\n", data_FIRSTCHANNEL_DOWN, data_FIRSTCHANNEL_UP);
          printf("Last channel              : %10u %10u\n", data_LASTCHANNEL_DOWN, data_LASTCHANNEL_UP);
          printf("Channel gaps              : ");
          for(tone=data_FIRSTCHANNEL_UP; tone<=data_LASTCHANNEL_DOWN; ++tone)
             if(data_GAPS[tone]) printf("%u ", tone);
          printf("\n");

          for(tone=0; tone<(ADR_TONE_END-ADR_TONE0); ++tone) {
             if(!(tone%16)) printf("\ntone %3u-%3u:", (2*tone), (2*tone)+31);
             printf(" %02x",  (16*(data_TONE[tone]%16))+(data_TONE[tone]/16));
          }; printf("\n");
          break;

       case 'd':
          displayAt(0x145);
          displayAt(0x150);
          displayAt(0x152);
          displayAt(0x2ee);
          displayAt(0x2f0);
          displayAt(0x2f2);
          printf("BitSum   (U, D) : (%7u, %7u)\n", data_BITSUM_UP, data_BITSUM_DOWN);
          printf("BitQuote (U, D) : (%5.5f, %5.5f)\n", data_BITQUOTE_UP, data_BITQUOTE_DOWN);
          break;

       default:
          printf("%s: unknown display method '%c'\n",argv[0], buffer[0]);
          return 1;
          break;
    }

  return 0;
}
