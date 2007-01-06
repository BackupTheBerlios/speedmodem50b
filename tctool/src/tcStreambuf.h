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
// $Id: tcStreambuf.h,v 1.7 2007/01/06 02:30:47 miunske Exp $

#ifndef _TCSTREAMBUF_h_included_
#define _TCSTREAMBUF_h_included_

#include "tcRtsDump.h"
#include "etherStreambuf.h"
#include "interface.h"
#include <string.h>
#include <vector>

namespace tc {
   typedef struct {
      UINT16_t unknown0x000;
      UINT16_t unknown0x002;
      char     bootbaseFwVersion[64];
      char     rasFwVersion[64];
      char     dslFwVersion[64];
      char     lineStandard[64];
      char     countryCode[4];
      char     macAddress[20];
      char     lineUptime[40];
      UINT16_t lineStatus;
      UINT16_t unknown0x146;
      UINT16_t bandwidthDownInterleaved;
      UINT16_t bandwidthDownFastpath;
      UINT16_t bandwidthUpInterleaved;
      UINT16_t bandwidthUpFastpath;
      UINT16_t unknown0x150;
      UINT16_t unknown0x152;
      UINT16_t lineErrorsFecDownFastpath;
      UINT16_t lineErrorsFecDownInterleaved;
      UINT16_t lineErrorsCrcDownFastpath;
      UINT16_t lineErrorsCrcDownInterleaved;
      UINT16_t lineErrorsHecDownFastpath;;
      UINT16_t lineErrorsHecDownInterleaved;
      UINT16_t lineErrorsFecUpFastpath;
      UINT16_t lineErrorsFecUpInterleaved;
      UINT16_t lineErrorsCrcUpFastpath;
      UINT16_t lineErrorsCrcUpInterleaved;
      UINT16_t lineErrorsHecUpFastpath;;
      UINT16_t lineErrorsHecUpInterleaved;
      UINT16_t lineRelativeLoadDown;
      UINT16_t lineNoiseMarginDown;
      UINT16_t lineTxPowerUp;
      UINT16_t lineAttenuationDown;
      UINT16_t lineRelativeLoadUp;
      UINT16_t lineNoisemarginUp;
      UINT16_t lineTxPowerDown;
      UINT16_t lineAttenuationUp;
      char     bitloadingValues[256];
      UINT16_t unknown0x27c;      // wan adsl p ?
      UINT16_t unknown0x27e;
      UINT16_t unknown0x280;
      UINT16_t unknown0x282;
      UINT16_t unknown0x284;
      UINT16_t unknown0x286;
      char     hostname[32];
      char     ipaddr[32];
      char     netmask[32];
      UINT16_t vcEncap;
      UINT16_t vcVPI;
      UINT16_t vcVCI;
      UINT16_t toneCount;     // highly guessed
      UINT16_t unknown0x2f0;
      UINT16_t unknown0x2f2;
      char     sourceIP[16];
   } tcStatusPacket;

   /** 
    * @class tcStreambuf
    *
    * @brief This is a streambuf to provide direct io via any ethernet-interface.
    *
    * This class provides a comfortable interface for ethernet-based console-io
    * with Tr*ndCh*p-devices using any ethernet-device.
    * It should always be used by it's wrapper-class tcStream.
    *
    * @see tcStream
    * @see etherStream
    * @see etherStreambuf
    *
    * @author Ralf Miunske <miunske at users.berlios.de>
    *
    * $Header $
    */
   class tcStreambuf : public etherStreambuf {
      public:
         static const unsigned long int tcFrameType  = 0xaaaa;
         static const unsigned int tcMinPacketLength = 0x40;

         /** 
          * @class tcPktType
          *
          * @brief This class encapsulates some Tr*ndCh*p-consts.
          *
          * This class encapsulates some Tr*ndCh*p-consts.
          *
          * @see tcStreambuf
          *
          * @author Ralf Miunske <miunske at users.berlios.de>
          *
          * $Header $
          */
         class tcPktType {
            public:
               static const char discover      = 0x01;
               static const char confirm       = 0x02;
               static const char stopSession   = 0x03;
               static const char startDump     = 0x04;
               static const char startConsole  = 0x05;
               static const char sendConsole   = 0x06;
               static const char recvConsole   = 0x07;
               static       char confirmation(char pktType);
         };

         /** 
          * @class discoveryResult
          *
          * @brief This class encapsulates information about one device gathered by tcStream::discover().
          *
          * This class contains the mac- and ip-address of
          * one device found using tcStream::discover().
          *
          * @see tcStream
          * @see tcStreambuf
          *
          * @author Ralf Miunske <miunske at users.berlios.de>
          *
          * $Header $
          */
         class discoveryResult {
            public:
               std::string mac;
               std::string ip;
               std::string password;
         };

         /** 
          * @class tcStatus
          *
          * @brief This class encapsulates status-information about one device gathered by tcStream::readStatus().
          *
          * This class holds all Information contained in the status-packet
          * retrieved from a modem via udp and provides methods to read them.
          * 
          * @see tcStream
          * @see tcStreambuf
          *
          * @author Ralf Miunske <miunske at users.berlios.de>
          *
          * $Header $
          */
         class tcStatus {
            public:
               static const unsigned int lineStatusDown         = 0x0000;
               static const unsigned int lineStatusWaiting      = 0x0800;
               static const unsigned int lineStatusInitializing = 0x1000;
               static const unsigned int lineStatusUp           = 0x2006;
               static const unsigned int vcEncapLLCSNAP         = 0x0001;
               static const unsigned int vcEncapVcMux           = 0x0002;
               static const double       protocolOverhead       = 0.1439;

            protected:
               tcStatusPacket status;
               bool           valid;

            public:
               tcStatus();
               tcStatus(const tcStatusPacket& status);

               int         getUnknown0x000() const;
               int         getUnknown0x002() const;
               std::string getBootbaseFwVersion() const;
               std::string getRasFwVersion() const;
               std::string getDslFwVersion() const;
               std::string getLineStandard() const;
               std::string getCountryCode() const;
               std::string getMacAddress() const;
               std::string getLineUptime() const;
               int         getLineStatus() const;
               int         getUnknown0x146() const;
               int         getBandwidthDown() const;
               int         getBandwidthUp() const;
               int         getBandwidthDownMax() const;
               int         getBandwidthUpMax() const;
               bool        isInterleaved() const;
               bool        isFastpath() const;
               int         getBandwidthDownInterleaved() const;
               int         getBandwidthDownFastpath() const;
               int         getBandwidthUpInterleaved() const;
               int         getBandwidthUpFastpath() const;
               int         getNettoBandwidthDown() const;
               int         getNettoBandwidthUp() const;
               int         getNettoBandwidthDownMax() const;
               int         getNettoBandwidthUpMax() const;
               int         getNettoBandwidthDownInterleaved() const;
               int         getNettoBandwidthDownFastpath() const;
               int         getNettoBandwidthUpInterleaved() const;
               int         getNettoBandwidthUpFastpath() const;
               int         getUnknown0x150() const;
               int         getUnknown0x152() const;
               int         getLineErrorsFecDownFastpath() const;
               int         getLineErrorsFecDownInterleaved() const;
               int         getLineErrorsCrcDownFastpath() const;
               int         getLineErrorsCrcDownInterleaved() const;
               int         getLineErrorsHecDownFastpath() const;
               int         getLineErrorsHecDownInterleaved() const;
               int         getLineErrorsFecUpFastpath() const;
               int         getLineErrorsFecUpInterleaved() const;
               int         getLineErrorsCrcUpFastpath() const;
               int         getLineErrorsCrcUpInterleaved() const;
               int         getLineErrorsHecUpFastpath() const;
               int         getLineErrorsHecUpInterleaved() const;
               int         getLineRelativeLoadDown() const;
               float       getLineNoiseMarginDown() const;
               float       getLineTxPowerUp() const;
               float       getLineAttenuationDown() const;
               int         getLineRelativeLoadUp() const;
               float       getLineNoisemarginUp() const;
               float       getLineTxPowerDown() const;
               float       getLineAttenuationUp() const;
               int         getBitloadingValue(int tone) const;
               int         getUnknown0x27c() const;
               int         getUnknown0x27e() const;
               int         getUnknown0x280() const;
               int         getUnknown0x282() const;
               int         getUnknown0x284() const;
               int         getUnknown0x286() const;
               std::string getHostname() const;
               std::string getIpaddr() const;
               std::string getNetmask() const;
               int         getVcEncap() const;
               int         getVcVPI() const;
               int         getVcVCI() const;
               int         getToneCount() const;
               int         getUnknown0x2f0() const;
               int         getUnknown0x2f2() const;
               std::string getSourceIP() const;
               bool        isValid() const;
         };

      protected:
         int maxRetries;
         discoveryResult peer;
         bool consoleSessionIsOpen;
         tcRtsDump* macRtsDump;
         tcStatus peerStatus;

      public:
         tcStreambuf();
         bool openInterface(const std::string& iface);
         virtual void closeInterface();
         void setMaxRetries(int retries);
         virtual bool sendRawPacket(etherPacket& pkt, int pktLength);
         const std::vector<tcStreambuf::discoveryResult>& macDiscover();
         const std::vector<tcStreambuf::discoveryResult>& udpDiscover(const std::string& ip = std::string());
         const std::vector<discoveryResult>& discover();
         const discoveryResult& setPeer(const discoveryResult& newPeer);
         const discoveryResult& setPeerByMac(const std::string& peerMac);
         const discoveryResult& setPeerByIp(const std::string& peerIp);
         void setPeerPassword(const std::string& password);
         bool openConsoleSession();
         bool isConsoleSessionOpen();
         tcRtsDump* startMacRtsDump();
         bool isMacRtsDumpRunning();
         bool closeSession(tcRtsDump* openRtsDump = NULL);
         bool login(const std::string& password = std::string());
         bool isLoggedIn();
         bool logout();
         bool executeCommand(const std::string& command, std::string& result, const std::string& password = std::string());
         std::vector<tcStatus> readStatus(std::string ip = std::string());
         bool updatePeerStatus();
         const tcStatus& getPeerStatus();

      protected:
         virtual void initValues();
         virtual bool receiveRawPacket(etherPacket& pkt, int& pktLength);
         void sendDiscoveryPacket();
         bool receiveDiscoveryPacket(std::string& mac, std::string& ip);
         void unique(std::vector<discoveryResult> &discoveries);
         bool getNextPacketIt(std::vector<etherPacket>::iterator& pktIt,
                              std::vector<int>::iterator& sizeIt, int pktType);
         bool getFirstPacketIt(std::vector<etherPacket>::iterator& pktIt,
                              std::vector<int>::iterator& sizeIt, int pktType);
         bool getNextPacketOfType(etherPacket& pkt, int& pktLength, int pktType);
         int  peekNextPacketOfTypeSize(int pktType);
         virtual bool getNextDataPacket(etherPacket& pkt, int& pktLength);
         virtual int  peekNextDataPacketSize();
         void sendOpenConsolePacket(const std::string& mac);
         bool receiveOpenConsolePacket(const std::string& mac);
         void sendStartMacRtsDumpPacket(const std::string& mac);
         bool receiveStartMacRtsDumpPacket(const std::string& mac);
         void sendCloseSessionPacket(const std::string& mac);
         bool receiveCloseSessionPacket(const std::string& mac);
         virtual std::streamsize showmanyc();
         void byteFix(tcStatusPacket &status);
         std::string trim(const std::string& str);
   };
};

#endif
