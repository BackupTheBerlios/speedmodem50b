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
// $Id: example.cpp,v 1.2 2006/12/06 17:48:25 miunske Exp $

#include "tcStream.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv) {
   /***********************************************************
    * Let's make a new tcStream-object
    ***********************************************************/
   tc::tcStream* tcModem = new tc::tcStream();

   /***********************************************************
    * First open the interface. Without that nothing will work.
    ***********************************************************/
   tcModem->openInterface("eth0");

   /***********************************************************
    * Then try to discover available modems
    ***********************************************************/
   printf("discovering...");
   std::vector<tc::tcStreambuf::discoveryResult> discovery = tcModem->discover();

   /***********************************************************
    * Continue, if there are modems on the line
    ***********************************************************/
   if(discovery.size()>0) {
      printf("found %u modem(s):\n", discovery.size());
      for(int i=0; i<discovery.size(); ++i)
         printf("%2u: IP: %s, MAC: %s\n", i, discovery[i].ip.c_str(), discovery[i].mac.c_str());


      /********************************************************
      * Now select the first modem
      ********************************************************/
      printf("\nselecting modem '0'.\n");
      tcModem->setPeer(discovery[0]);

      /********************************************************
      * and open the console session for it.
      ********************************************************/
      tcModem->openConsoleSession();

      /********************************************************
       * Send some data
       ********************************************************/
      (*tcModem) << " \r\n";

      /********************************************************
       * Read some data
       ********************************************************/
      char data[2048];
      int  dataLen;
      while((dataLen = tcModem->readsome(data, 2048))>0) {
         printf("printing %u bytes:\n", dataLen);
         for(int i=0; i<dataLen; ++i)
            printf("%c", data[i]);
         printf("\n");
      }
      printf("\n");

      /********************************************************
      * Say good bye!
      ********************************************************/
      tcModem->closeSession();

      /********************************************************
      * Read status via udp
      ********************************************************/
      tcModem->updatePeerStatus();
      if(tcModem->getPeerStatus().isValid()) {
         printf("Unknown0x000                 : %4u\n", tcModem->getPeerStatus().getUnknown0x000());
         printf("Unknown0x002                 : %4u\n", tcModem->getPeerStatus().getUnknown0x002());
         printf("BootbaseFwVersion            : %s\n", tcModem->getPeerStatus().getBootbaseFwVersion().c_str());
         printf("RasFwVersion                 : %s\n", tcModem->getPeerStatus().getRasFwVersion().c_str());
         printf("DslFwVersion                 : %s\n", tcModem->getPeerStatus().getDslFwVersion().c_str());
         printf("LineStandard                 : %s\n", tcModem->getPeerStatus().getLineStandard().c_str());
         printf("CountryCode                  : %s\n", tcModem->getPeerStatus().getCountryCode().c_str());
         printf("MacAddress                   : %s\n", tcModem->getPeerStatus().getMacAddress().c_str());
         printf("LineUptime                   : %s\n", tcModem->getPeerStatus().getLineUptime().c_str());
         printf("LineStatus                   : ");
         switch(tcModem->getPeerStatus().getLineStatus()) {
            case tc::tcStreambuf::tcStatus::lineStatusDown:
               printf("down\n");
               break;
            case tc::tcStreambuf::tcStatus::lineStatusWaiting:
               printf("waiting\n");
               break;
            case tc::tcStreambuf::tcStatus::lineStatusInitializing:
               printf("initializing\n");
               break;
            case tc::tcStreambuf::tcStatus::lineStatusUp:
               printf("up\n");
               break;
            default:
               printf("unknown\n");
               break;
         }
         printf("Unknown0x146                 : %4u\n", tcModem->getPeerStatus().getUnknown0x146());
         printf("BandwidthDown                : %4u\n", tcModem->getPeerStatus().getBandwidthDown());
         printf("BandwidthUp                  : %4u\n", tcModem->getPeerStatus().getBandwidthUp());
         printf("BandwidthDownInterleaved     : %4u\n", tcModem->getPeerStatus().getBandwidthDownInterleaved());
         printf("BandwidthDownFastpath        : %4u\n", tcModem->getPeerStatus().getBandwidthDownFastpath());
         printf("BandwidthUpInterleaved       : %4u\n", tcModem->getPeerStatus().getBandwidthUpInterleaved());
         printf("BandwidthUpFastpath          : %4u\n", tcModem->getPeerStatus().getBandwidthUpFastpath());
         printf("isInterleaved                : %4u\n", tcModem->getPeerStatus().isInterleaved());
         printf("isFastpath                   : %4u\n", tcModem->getPeerStatus().isFastpath());
         printf("Unknown0x150                 : %4u\n", tcModem->getPeerStatus().getUnknown0x150());
         printf("Unknown0x152                 : %4u\n", tcModem->getPeerStatus().getUnknown0x152());
         printf("LineErrorsFecDownFastpath    : %4u\n", tcModem->getPeerStatus().getLineErrorsFecDownFastpath());
         printf("LineErrorsFecDownInterleaved : %4u\n", tcModem->getPeerStatus().getLineErrorsFecDownInterleaved());
         printf("LineErrorsCrcDownFastpath    : %4u\n", tcModem->getPeerStatus().getLineErrorsCrcDownFastpath());
         printf("LineErrorsCrcDownInterleaved : %4u\n", tcModem->getPeerStatus().getLineErrorsCrcDownInterleaved());
         printf("LineErrorsHecDownFastpath    : %4u\n", tcModem->getPeerStatus().getLineErrorsHecDownFastpath());
         printf("LineErrorsHecDownInterleaved : %4u\n", tcModem->getPeerStatus().getLineErrorsHecDownInterleaved());
         printf("LineErrorsFecUpFastpath      : %4u\n", tcModem->getPeerStatus().getLineErrorsFecUpFastpath());
         printf("LineErrorsFecUpInterleaved   : %4u\n", tcModem->getPeerStatus().getLineErrorsFecUpInterleaved());
         printf("LineErrorsCrcUpFastpath      : %4u\n", tcModem->getPeerStatus().getLineErrorsCrcUpFastpath());
         printf("LineErrorsCrcUpInterleaved   : %4u\n", tcModem->getPeerStatus().getLineErrorsCrcUpInterleaved());
         printf("LineErrorsHecUpFastpath      : %4u\n", tcModem->getPeerStatus().getLineErrorsHecUpFastpath());
         printf("LineErrorsHecUpInterleaved   : %4u\n", tcModem->getPeerStatus().getLineErrorsHecUpInterleaved());
         printf("LineRelativeLoadDown         : %3u%\n", tcModem->getPeerStatus().getLineRelativeLoadDown());
         printf("LineNoiseMarginDown          : %4.1f\n", tcModem->getPeerStatus().getLineNoiseMarginDown());
         printf("LineTxPowerUp                : %4.1f\n", tcModem->getPeerStatus().getLineTxPowerUp());
         printf("LineAttenuationDown          : %4.1f\n", tcModem->getPeerStatus().getLineAttenuationDown());
         printf("LineRelativeLoadUp           : %3u%\n", tcModem->getPeerStatus().getLineRelativeLoadUp());
         printf("LineNoisemarginUp            : %4.1f\n", tcModem->getPeerStatus().getLineNoisemarginUp());
         printf("LineTxPowerDown              : %4.1f\n", tcModem->getPeerStatus().getLineTxPowerDown());
         printf("LineAttenuationUp            : %4.1f\n", tcModem->getPeerStatus().getLineAttenuationUp());
         printf("BitloadingValue95            : %4u\n", tcModem->getPeerStatus().getBitloadingValue(95));
         printf("BitloadingValue96            : %4u\n", tcModem->getPeerStatus().getBitloadingValue(96));
         printf("BitloadingValue97            : %4u\n", tcModem->getPeerStatus().getBitloadingValue(97));
         printf("Unknown0x27c                 : %4u\n", tcModem->getPeerStatus().getUnknown0x27c());
         printf("Unknown0x27e                 : %4u\n", tcModem->getPeerStatus().getUnknown0x27e());
         printf("Unknown0x280                 : %4u\n", tcModem->getPeerStatus().getUnknown0x280());
         printf("Unknown0x282                 : %4u\n", tcModem->getPeerStatus().getUnknown0x282());
         printf("Unknown0x284                 : %4u\n", tcModem->getPeerStatus().getUnknown0x282());
         printf("Unknown0x286                 : %4u\n", tcModem->getPeerStatus().getUnknown0x286());
         printf("Hostname                     : %s\n", tcModem->getPeerStatus().getHostname().c_str());
         printf("Ipaddr                       : %s\n", tcModem->getPeerStatus().getIpaddr().c_str());
         printf("Netmask                      : %s\n", tcModem->getPeerStatus().getNetmask().c_str());
         printf("VcQOS                        : %4u\n", tcModem->getPeerStatus().getVcQOS());
         printf("VcVPI                        : %4u\n", tcModem->getPeerStatus().getVcVPI());
         printf("VcVCI                        : %4u\n", tcModem->getPeerStatus().getVcVCI());
         printf("ToneCount                    : %4u\n", tcModem->getPeerStatus().getToneCount());
         printf("Unknown0x2f0                 : %4u\n", tcModem->getPeerStatus().getUnknown0x2f0());
         printf("Unknown0x2f2                 : %4u\n", tcModem->getPeerStatus().getUnknown0x2f2());
         printf("udpSourceIP                  : %s\n", tcModem->getPeerStatus().getSourceIP().c_str());
      }
   } else // (discovery.size()>0)
      printf("no modems found!\n");

   /***********************************************************
    * Don't forget to close the interface
    ***********************************************************/
   tcModem->closeInterface();

   /***********************************************************
    * kill the object, everything cleans up automatically
    ***********************************************************/
   delete tcModem;

   return 0;
}
