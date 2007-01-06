/***************************************************************************
 *   Copyright (C) 2005 by Ralf Miunske                                    *
 *   miunske at users.berlios.de                                           *
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
 *   tctool for linux                                                      *
 *   Tr*ndCh*p-utility                                                     *
 *                                                                         *
 *   LIC: GPL                                                              *
 *                                                                         *
 ***************************************************************************/
// $Id: tctool.cpp,v 1.8 2007/01/06 02:30:47 miunske Exp $

#define BUFFERSIZE 8192

#include "tcStream.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

std::string                      options = "I:m:i:r:t:p:c:MUdsSCRh";
std::string                      ethDev  = "eth0";
tc::tcStreambuf::discoveryResult target;
std::string                      targetPassword;
tc::tcStream                     tcDev;
tc::tcRtsDump*                   tcDump;
char                             *binary;

void printHelp(char *binary) {
   fprintf(stderr, "\ntcTool\n\n");
   fprintf(stderr, "usage: %s {-(I dev|m mac|i ip|r retr|t time|c cmd)} -(M|U|d|s|S|C|R|h)\n", binary);
   fprintf(stderr, "where:\n");
   fprintf(stderr, "   -I dev  : sets the iface for direct mac-communication via 'dev'\n");
   fprintf(stderr, "   -m mac  : sets the target-mac of the modem for console-communication.\n");
   fprintf(stderr, "   -i ip   : sets the IP used for status-requests and udp-discovery to 'ip'\n");
   fprintf(stderr, "   -r retr : sets the number of retries to resend a packet if it gets not acknowledged.\n");
   fprintf(stderr, "   -t time : sets the acknowledge-timeout to 'time' in ms before retrying.\n");
   fprintf(stderr, "   -p pass : sets the password for accessing the device's console-service.\n");
   fprintf(stderr, "   -M      : starts a mac-discovery.\n");
   fprintf(stderr, "   -U      : starts a udp-discovery.\n");
   fprintf(stderr, "   -d      : starts a combined discovery.\n");
   fprintf(stderr, "   -s      : retrieves the udp-status from the modem and displays it.\n");
   fprintf(stderr, "   -S      : same a above, but script-readable.\n");
   fprintf(stderr, "   -C      : connects to the modem for a console-session.\n");
   fprintf(stderr, "   -R      : enables the real-time-status dump via ethernet.\n");
   fprintf(stderr, "   -c cmd  : executes a command in a console-session. password required.\n");
   fprintf(stderr, "   -h      : displays this cruft.\n\n");
}

int errorMsg(char *binary, int error) {
   if(error!=0) fprintf(stderr, "%s: ", binary);
   switch(error) {
      case -SIGHUP:
         fprintf(stderr, "caught SIGHUP. terminating cleanly.\n");
         break;
      case -SIGINT:
         fprintf(stderr, "caught SIGINT. terminating cleanly.\n");
         break;
      case -SIGQUIT:
         fprintf(stderr, "caught SIGQUIT. terminating cleanly.\n");
         break;
      case -SIGABRT:
         fprintf(stderr, "caught SIGABRT. terminating cleanly.\n");
         break;
      case -SIGTERM:
         fprintf(stderr, "caught SIGTERM. terminating cleanly.\n");
         break;
      case -SIGTSTP:
         fprintf(stderr, "caught SIGTSTP. terminating cleanly.\n");
         break;
      case -SIGUSR1:
         fprintf(stderr, "caught SIGUSR1. terminating cleanly.\n");
         break;
      case -SIGUSR2:
         fprintf(stderr, "caught SIGUSR2. terminating cleanly.\n");
         break;
      case 0:
         break;
      case 1:
         fprintf(stderr, "no action given. try -h.\n");
         break;
      case 2:
         fprintf(stderr, "cannot open interface %s.\n", ethDev.c_str());
         break;
      case 3:
         fprintf(stderr, "retries must be an integer.\n");
         break;
      case 4:
         fprintf(stderr, "retries must be above 1.\n");
         break;
      case 5:
         fprintf(stderr, "timeout has to be an integer.\n");
         break;
      case 6:
         fprintf(stderr, "no devices could be discovered.\n");
         break;
      case 7:
         fprintf(stderr, "no devices could be queried.\n");
         break;
      case 8:
         fprintf(stderr, "no device to connect to, plase specify target mac.\n");
         break;
      case 9:
         fprintf(stderr, "more than one device to connect to, plase specify target mac.\n");
         break;
      case 10:
         fprintf(stderr, "cannot open console-session with %s.\n", target.mac.c_str());
         break;
      case 11:
         fprintf(stderr, "cannot log in.\n", error);
         break;
      case 12:
         fprintf(stderr, "cannot log out.\n", error);
         break;
      case 13:
         fprintf(stderr, "password required for remote commands.\n");
         break;
      case 14:
         fprintf(stderr, "cannot execute command.\n");
         break;
      case 15:
         fprintf(stderr, "cannot start MAC RTS-Dump.\n");
         break;
      default:
         fprintf(stderr, "unknown error %u.\n", error);
         break;
   }

   if(tcDev.isInterfaceOpen() && (tcDev.isConsoleSessionOpen() || tcDev.isMacRtsDumpRunning())) {
      if(tcDev.isConsoleSessionOpen() && tcDev.isLoggedIn()) tcDev.logout();
      tcDev.closeSession(tcDump);
   }

   tcDev.closeInterface();
   return error;
}

void signalHandler(int sig) {
   exit(errorMsg(binary, -sig));
}

void installSignalHandler(char* argv0) {
   binary=argv0;
   sigset_t mask;
   struct sigaction action;
   sigemptyset( &mask );
   action.sa_mask = mask;
   action.sa_flags = SA_NODEFER;
   action.sa_handler = signalHandler;
   sigaction(SIGHUP , &action, NULL);
   sigaction(SIGINT , &action, NULL);
   sigaction(SIGQUIT, &action, NULL);
   sigaction(SIGABRT, &action, NULL);
   sigaction(SIGTERM, &action, NULL);
   sigaction(SIGTSTP, &action, NULL);
   sigaction(SIGUSR1, &action, NULL);
   sigaction(SIGUSR2, &action, NULL);
}

int setInterface(char* interface) {
   ethDev=std::string(interface);
   return tcDev.openInterface(ethDev)?0:2;
}

int setTargetMac(char* destMac) {
   target.mac=std::string(destMac);
   return 0;
}

int setTargetIP(char* destIP) {
   target.ip=std::string(destIP);
   return 0;
}

int setRetries(char* retries) {
   int intRetries;
   if(sscanf(retries, "%u", &intRetries)!=1) return 3;
   if(intRetries<1) return 4;
   tcDev.setMaxRetries(intRetries);
   return 0;
}

int setTimeout(char* timeout) {
   int intTimeout;
   if(sscanf(timeout, "%i", &intTimeout)!=1) return 5;
   tcDev.setRxTimeout(intTimeout);
   return 0;
}

int setPassword(char * password) {
   targetPassword=std::string(password);
   return 0;
}

int printDiscovery(const std::vector<tc::tcStreambuf::discoveryResult>& discovery) {
   int result = 6;
   std::vector<tc::tcStreambuf::discoveryResult>::const_iterator dIt;
   for(dIt=discovery.begin(); dIt!=discovery.end(); ++dIt, result=0)
      printf("%s %s\n", dIt->mac.c_str(), dIt->ip.c_str());
   return result;
}

int doMacDiscovery() {
   if(!tcDev.isInterfaceOpen())
      if(!tcDev.openInterface(ethDev))
         return 2;

   return printDiscovery(tcDev.macDiscover());
}

int doUdpDiscovery() {
   if(!tcDev.isInterfaceOpen())
      if(!tcDev.openInterface(ethDev))
         return 2;

   return printDiscovery(tcDev.udpDiscover(target.ip));
}

int doDiscovery() {
   if(!tcDev.isInterfaceOpen())
      if(!tcDev.openInterface(ethDev))
         return 2;

   return printDiscovery(tcDev.discover());
}


void printStatusHumanReadable(const tc::tcStreambuf::tcStatus& status, int idx, int max) {
   printf("Firmware Information:\n");
   printf("Bootbase Version : %s\n",   status.getBootbaseFwVersion().c_str());
   printf("RAS F/W Version  : %s\n",   status.getRasFwVersion().c_str());
   printf("DSL FW Version   : %s\n",   status.getDslFwVersion().c_str());
   printf("Standard         : %s\n",   status.getLineStandard().c_str());
   printf("Country Code     : %s\n",   status.getCountryCode().c_str());
   printf("MAC Address      : %s\n",   status.getMacAddress().c_str());
   printf("Hostname         : %s\n\n", status.getHostname().c_str());

   printf("ADSL Status:\n");
   printf("LINE Status : ");
   switch(status.getLineStatus()) {
      case tc::tcStreambuf::tcStatus::lineStatusDown:
         printf("down");
         break;
      case tc::tcStreambuf::tcStatus::lineStatusWaiting:
         printf("waiting for initialisation");
         break;
      case tc::tcStreambuf::tcStatus::lineStatusInitializing:
         printf("initialising");
         break;
      case tc::tcStreambuf::tcStatus::lineStatusUp:
         printf("up");
         break;
      default:
         printf("unknown");
         break;
   }
   printf("\n");
   printf("LINE Uptime : %s\n", status.getLineUptime().c_str());
   printf("ATM vc      : %u/%u ", status.getVcVPI(), status.getVcVCI());
   switch(status.getVcEncap()) {
      case tc::tcStreambuf::tcStatus::vcEncapLLCSNAP:
         printf("LLC/SNAP");
         break;
      case tc::tcStreambuf::tcStatus::vcEncapVcMux:
         printf("VcMux");
         break;
      default:
         printf("unknown");
         break;
   }
   printf("\n");

   printf("                                down         up\n");
   if(!(status.isFastpath()||status.isInterleaved()))
      printf("Bit-rate                  :  %10u  %10u\n", 0, 0);
   if(status.isFastpath())
      printf("Bit-rate  (fast)          :  %10u  %10u\n", status.getBandwidthDownFastpath(), status.getBandwidthUpFastpath());
   if(status.isInterleaved())
      printf("Bit-rate  (interleaved)   :  %10u  %10u\n", status.getBandwidthDownInterleaved(), status.getBandwidthUpInterleaved());
   printf("Bit-rate  (relative cap.) :  %8u %%  %8u %%\n", status.getLineRelativeLoadDown(), status.getLineRelativeLoadUp());
   printf("Bit-rate  (max)           :  %10u  %10u\n", status.getBandwidthDownMax(), status.getBandwidthUpMax());

   if(!(status.isFastpath()||status.isInterleaved()))
      printf("Data-rate                 :  %10u  %10u\n", 0, 0);
   if(status.isFastpath())
      printf("Data-rate (fast)          :  %10u  %10u\n", status.getNettoBandwidthDownFastpath(), status.getNettoBandwidthUpFastpath());
   if(status.isInterleaved())
      printf("Data-rate (interleaved)   :  %10u  %10u\n", status.getNettoBandwidthDownInterleaved(), status.getNettoBandwidthUpInterleaved());

   if(!(status.isFastpath()||status.isInterleaved())) {
      printf("FEC error                 :  %10u  %10u\n", 0,0);
      printf("CRC error                 :  %10u  %10u\n", 0, 0);
      printf("HEC error                 :  %10u  %10u\n", 0, 0);
   }
   if(status.isFastpath()) {
      printf("FEC error (fast)          :  %10u  %10u\n", status.getLineErrorsFecDownFastpath(), status.getLineErrorsFecUpFastpath());
      printf("CRC error (fast)          :  %10u  %10u\n", status.getLineErrorsCrcDownFastpath(), status.getLineErrorsCrcUpFastpath());
      printf("HEC error (fast)          :  %10u  %10u\n", status.getLineErrorsHecDownFastpath(), status.getLineErrorsHecUpFastpath());
   }
   if(status.isInterleaved()) {
      printf("FEC error (interleaved)   :  %10u  %10u\n", status.getLineErrorsFecDownInterleaved(), status.getLineErrorsFecUpInterleaved());
      printf("CRC error (interleaved)   :  %10u  %10u\n", status.getLineErrorsCrcDownInterleaved(), status.getLineErrorsCrcUpInterleaved());
      printf("HEC error (interleaved)   :  %10u  %10u\n", status.getLineErrorsHecDownInterleaved(), status.getLineErrorsHecUpInterleaved());
   }

   printf("Noise margin              :   %6.1f dB   %6.1f dB \n", status.getLineNoiseMarginDown(), status.getLineNoisemarginUp());
   printf("Attenuation               :   %6.1f dB   %6.1f dB \n", status.getLineAttenuationDown(), status.getLineAttenuationUp());
   printf("Transmit power            :   %6.1f dBm  %6.1f dBm\n", status.getLineTxPowerDown(), status.getLineTxPowerUp());

   for(int tone=0; tone<(status.getToneCount()/2); ++tone) {
      if(!(tone%16)) printf("\ntone %3u-%3u:", (2*tone), (2*tone)+31);
      printf(" %02x",  (16*status.getBitloadingValue(2*tone))+status.getBitloadingValue(2*tone+1));
   }; printf("\n");
   return;
}

void printStatusScriptable(const tc::tcStreambuf::tcStatus& status, int idx) {
   if(status.isValid()) {
      printf("tcHW_FWVER_BOOTBASE='%s'\n", status.getBootbaseFwVersion().c_str());
      printf("tcHW_FWVER_RAS='%s'\n", status.getRasFwVersion().c_str());
      printf("tcHW_FWVER_DSL='%s'\n", status.getDslFwVersion().c_str());
      printf("tcLINE_STANDARD='%s'\n", status.getLineStandard().c_str());
      printf("tcLINE_COUNTRY='%s'\n", status.getCountryCode().c_str());
      printf("tcHW_MACADDR='%s'\n", status.getMacAddress().c_str());
      printf("tcLINE_UPTIME='%s'\n", status.getLineUptime().c_str());
      printf("tcLINE_STATUS='");
      switch(status.getLineStatus()) {
         case tc::tcStreambuf::tcStatus::lineStatusDown:
            printf("DOWN");
            break;
         case tc::tcStreambuf::tcStatus::lineStatusWaiting:
            printf("WAITING_FOR_INITIALIZATION");
            break;
         case tc::tcStreambuf::tcStatus::lineStatusInitializing:
            printf("INITIALIZING");
            break;
         case tc::tcStreambuf::tcStatus::lineStatusUp:
            printf("UP");
            break;
         default:
            printf("unknown");
            break;
      }
      printf("'\n");
      printf("tcLINE_BW_DOWN='%u'\n", status.getBandwidthDown());
      printf("tcLINE_BW_UP='%u'\n", status.getBandwidthUp());
      printf("tcLINE_BW_DOWN_MAX='%u'\n", status.getBandwidthDownMax());
      printf("tcLINE_BW_UP_MAX='%u'\n", status.getBandwidthUpMax());
      printf("tcLINE_BW_DOWN_INTERLEAVED='%u'\n", status.getBandwidthDownInterleaved());
      printf("tcLINE_BW_DOWN_FASTPATH='%u'\n", status.getBandwidthDownFastpath());
      printf("tcLINE_BW_UP_INTERLEAVED='%u'\n", status.getBandwidthUpInterleaved());
      printf("tcLINE_BW_UP_FASTPATH='%u'\n", status.getBandwidthUpFastpath());
      printf("tcLINE_ISINTERLEAVED='%u'\n", status.isInterleaved());
      printf("tcLINE_ISFASTPATH='%u'\n", status.isFastpath());
      printf("tcLINE_ERR_FEC_DOWN_FASTPATH='%u'\n", status.getLineErrorsFecDownFastpath());
      printf("tcLINE_ERR_FEC_DOWN_INTERLEAVED='%u'\n", status.getLineErrorsFecDownInterleaved());
      printf("tcLINE_ERR_CRC_DOWN_FASTPATH='%u'\n", status.getLineErrorsCrcDownFastpath());
      printf("tcLINE_ERR_CRC_DOWN_INTERLEAVED='%u'\n", status.getLineErrorsCrcDownInterleaved());
      printf("tcLINE_ERR_HEC_DOWN_FASTPATH='%u'\n", status.getLineErrorsHecDownFastpath());
      printf("tcLINE_ERR_HEC_DOWN_INTERLEAVED='%u'\n", status.getLineErrorsHecDownInterleaved());
      printf("tcLINE_ERR_FEC_UP_FASTPATH='%u'\n", status.getLineErrorsFecUpFastpath());
      printf("tcLINE_ERR_FEC_UP_INTERLEAVED='%u'\n", status.getLineErrorsFecUpInterleaved());
      printf("tcLINE_ERR_CRC_UP_FASTPATH='%u'\n", status.getLineErrorsCrcUpFastpath());
      printf("tcLINE_ERR_CRC_UP_INTERLEAVED='%u'\n", status.getLineErrorsCrcUpInterleaved());
      printf("tcLINE_ERR_HEC_UP_FASTPATH='%u'\n", status.getLineErrorsHecUpFastpath());
      printf("tcLINE_ERR_HEC_UP_INTERLEAVED='%u'\n", status.getLineErrorsHecUpInterleaved());
      printf("tcLINE_RELLOAD_DOWN='%u%%'\n", status.getLineRelativeLoadDown());
      printf("tcLINE_NOISE_DOWN='%.1f'\n", status.getLineNoiseMarginDown());
      printf("tcLINE_XMITPWR_UP='%.1f'\n", status.getLineTxPowerUp());
      printf("tcLINE_ATT_DOWN='%.1f'\n", status.getLineAttenuationDown());
      printf("tcLINE_RELLOAD_UP='%u%%'\n", status.getLineRelativeLoadUp());
      printf("tcLINE_NOISE_UP='%.1f'\n", status.getLineNoisemarginUp());
      printf("tcLINE_XMITPWR_DOWN='%.1f'\n", status.getLineTxPowerDown());
      printf("tcLINE_ATT_UP='%.1f'\n", status.getLineAttenuationUp());
      printf("tcLINE_TONES='%u", status.getBitloadingValue(0));
      for(int tone=1; tone<status.getToneCount(); ++tone)
         printf(",%u", status.getBitloadingValue(tone));
      printf("'\n");
      printf("tcHOSTNAME='%s'\n", status.getHostname().c_str());
      printf("tcLAN_IP='%s'\n", status.getIpaddr().c_str());
      printf("tcLAN_NETMASK='%s'\n", status.getNetmask().c_str());
      printf("tcVC_ENCAP='");
      switch(status.getVcEncap()) {
         case tc::tcStreambuf::tcStatus::vcEncapLLCSNAP:
            printf("LLCSNAP");
            break;
         case tc::tcStreambuf::tcStatus::vcEncapVcMux:
            printf("VCMUX");
            break;
         default:
            printf("unknown");
            break;
      }
      printf("'\n");
      printf("tcVC_VPI='%u'\n", status.getVcVPI());
      printf("tcVC_VCI='%u'\n", status.getVcVCI());
      printf("tcLINE_TONECOUNT='%u'\n", status.getToneCount());
      printf("tcLAN_UDPSOURCEIP='%s'\n", status.getSourceIP().c_str());
   }
   return;
}

int getStatus(bool scriptable) {
   int result = 7;
   int idx=1;

   target.password=targetPassword;
   target=tcDev.setPeer(target);

   std::vector<tc::tcStreambuf::tcStatus> stats = tcDev.readStatus(target.ip);
   std::vector<tc::tcStreambuf::tcStatus>::iterator sIt;
   for(sIt=stats.begin(); sIt!=stats.end(); ++sIt, ++idx, result=0)
      if(sIt->isValid()) {
         if(scriptable)
            printStatusScriptable(*sIt, idx);
         else
            printStatusHumanReadable(*sIt, idx, stats.size());
      }

   return result;
}

int setTcDev() {
   if(!tcDev.isInterfaceOpen())
      if(!tcDev.openInterface(ethDev))
         return 2;

   target.password=targetPassword;
   target=tcDev.setPeer(target);

   if(target.mac.size()==0) {
      std::vector<tc::tcStreambuf::discoveryResult> discovery;
      discovery=tcDev.macDiscover();
      if(discovery.size()>0) {
         if(discovery.size()<2)
            target=discovery[0];
         else return 9;
      } else return 8;
      target.password=targetPassword;
      target=tcDev.setPeer(target);
   }
   return 0;
}

int startConsoleSession() {
   int openDevResult = 0;
   if((openDevResult=setTcDev())>0) return openDevResult;

   if(!tcDev.openConsoleSession()) return 10;

   if(target.password.size()>0) {
      if(!tcDev.login()) errorMsg("warning", 11);
   }

   ssize_t         stdinBytes = 1;
   std::streamsize tcDevBytes = 1;
   char buffer[BUFFERSIZE];

   int ifacefd = tcDev.getInterfaceFD();
   int stdinfd = fileno(stdin);
   struct timeval tv; fd_set fds;
   int i;

   fprintf(stderr, "connected to %s.\n", target.mac.c_str());
   tcDev << " \x0d";
   while(stdinBytes>0 && !feof(stdin)) {
      FD_ZERO(&fds);
      FD_SET(stdinfd, &fds);
      FD_SET(ifacefd, &fds);
      tv.tv_sec=0; tv.tv_usec=2500000;
      if(select(ifacefd+1, &fds, NULL, NULL, &tv)>0) {
         if(FD_ISSET(stdinfd, &fds)) {
            stdinBytes=read(stdinfd, buffer, BUFFERSIZE);
            for(i=0; i<stdinBytes; ++i)
               if(buffer[i]==10) buffer[i]=13;
            tcDev.write(buffer, stdinBytes);
         }
         if(FD_ISSET(ifacefd, &fds)) {
            tcDevBytes=tcDev.readsome(buffer, BUFFERSIZE-1);
            write(stdinfd, buffer, tcDevBytes);
         }
      }
   }
   printf("\n");

   if(!tcDev.logout()) errorMsg("warning", 12);

   tcDev.closeSession();
   return 0;
}

int executeCommand(char* command) {
   int openDevResult = 0;
   if(targetPassword.size()==0) return 13;
   if((openDevResult=setTcDev())>0) return openDevResult;
   if(!tcDev.openConsoleSession()) return 10;
   if(!tcDev.login()) return 11;

   int result = 0;
   std::string execResult;
   if(tcDev.executeCommand(std::string(command), execResult)) {
      for(std::string::iterator sIt=execResult.begin();
          sIt!=execResult.end(); ++sIt) putchar(*sIt);
   } else
      result=14;

   if(!tcDev.logout()) errorMsg("warning", 12);
   tcDev.closeSession();
   return result;
}

int startMacRtsDump() {
   int openDevResult = 0;
   if((openDevResult=setTcDev())>0) return openDevResult;

   int result = 0;
   tcDump = tcDev.startMacRtsDump();
   if(tcDump!=NULL) {
      sleep(1);
      for(int i=0; i<50 && tcDump->doSomething(); ++i);
   } else
      result=15;

   tcDev.closeSession(tcDump);
   return result;
}

int main(int argc, char** argv) {
   installSignalHandler(argv[0]);
   int optResult = 1;
   int mainResult = 1;
   int option;
   while((option = getopt(argc, argv, options.c_str())) != -1) {
      switch(option) {
         case 'I':
            if((optResult=setInterface(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 'm':
            if((optResult=setTargetMac(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 'i':
            if((optResult=setTargetIP(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 'r':
            if((optResult=setRetries(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 't':
            if((optResult=setTimeout(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 'p':
            if((optResult=setPassword(optarg))>0) return errorMsg(argv[0], optResult);
            break;
         case 'M':   // doMacDiscovery
            return errorMsg(argv[0], doMacDiscovery());
            break;
         case 'U':   // doUdpDiscovery
            return errorMsg(argv[0], doUdpDiscovery());
            break;
         case 'd':   // doDiscovery
            return errorMsg(argv[0], doDiscovery());
            break;
         case 'C':   // startConsoleSession
            return errorMsg(argv[0], startConsoleSession());
            break;
         case 'c':   // executeCommand
            if((mainResult=executeCommand(optarg))>0) return errorMsg(argv[0], mainResult);
            break;
         case 'R':   // startMacRtsDump
            return errorMsg(argv[0], startMacRtsDump());
            break;
         case 's':   // getStatusHumanReadable
            return errorMsg(argv[0], getStatus(false));
            break;
         case 'S':   // getStatusScriptable
            return errorMsg(argv[0], getStatus(true));
            break;
         case 'h':
            printHelp(argv[0]);
            return 0;
            break;
      }
   }
   return errorMsg(argv[0], mainResult);
}

