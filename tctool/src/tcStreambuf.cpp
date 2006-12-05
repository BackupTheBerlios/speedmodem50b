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
 *   tctool for linux                                                      *
 *   Tr*ndCh*p-utility                                                     *
 *                                                                         *
 *   LIC: GPL                                                              *
 *                                                                         *
 ***************************************************************************/
// $Id: tcStreambuf.cpp,v 1.3 2006/12/05 09:39:23 miunske Exp $

#include "tcStreambuf.h"

// this is for the udp-part...
#include <netdb.h>         // gethostbyname hostent
#include <arpa/inet.h>     // inet_ntoa

namespace tc {
   char tcStreambuf::tcPktType::confirmation(char pktType) {
      if(pktType<0 || pktType==confirm) return pktType;
      return -pktType;
   }

   tcStreambuf::tcStreambuf()
   {
      initValues();
   }

   void tcStreambuf::initValues() {
      etherStreambuf::initValues();
      macRtsDumpIsRunning=consoleSessionIsOpen=false;
      setTxMode(packetwise);
      setRxMode(stream);
      setRxTimeoutuSec(10000);
      setRxCollectTimeoutuSec(10000);
      setTxFrameType(tcFrameType);
      setMaxRetries(3);
      std::string header;
      header.push_back(tcPktType::sendConsole);
      setDataHeader(header);
      setMTU(254);
      peer.mac.clear();
      peer.ip.clear();
   }

   bool tcStreambuf::openInterface(const std::string& iface)  {
      return etherStreambuf::openInterface(iface, tcFrameType);
   }

   void tcStreambuf::closeInterface()  {
      if(isConsoleSessionOpen()) closeSession();
      etherStreambuf::closeInterface();
   }

   void tcStreambuf::setMaxRetries(int retries) {
      maxRetries=retries;
   }

   bool tcStreambuf::sendRawPacket(etherPacket& pkt, int pktLength) {
      if(pktLength<tcMinPacketLength) {
         memset(&pkt.raw[pktLength], 0, tcMinPacketLength-pktLength);
         pktLength=tcMinPacketLength;
      }
      return etherStreambuf::sendRawPacket(pkt, pktLength);
   }

   bool tcStreambuf::receiveRawPacket(etherPacket& pkt, int& pktLength) {
      if(etherStreambuf::receiveRawPacket(pkt, pktLength)) {
         if(pkt.frame.data[0]==tcPktType::confirm &&
            pkt.frame.data[1]==tcPktType::discover)
            return true;

         if(pkt.frame.data[0]==tcPktType::confirm &&
            (pkt.frame.data[1]==tcPktType::startConsole ||
             pkt.frame.data[1]==tcPktType::startDump) &&
            peer.mac.size()==0)
            return true;

         if(peer.mac!=unpackMac(pkt.frame.smac))
            return false;

         if(pkt.frame.data[0]==tcPktType::recvConsole)
            while(pktLength>0 && pkt.raw[pktLength-1]==0) --pktLength;

         return true;
      } else return false;
   }

   void tcStreambuf::sendDiscoveryPacket() {
      std::string data;
      data.clear(); data.push_back(tcPktType::discover);
      sendPacket(getIfaceMac(), getBroadcastMac(), tcFrameType, data);
   }

   bool tcStreambuf::receiveDiscoveryPacket(std::string& mac, std::string& ip) {
      etherPacket pkt;
      int         pktLength;
      bool result = getNextPacketOfType(pkt, pktLength, tcPktType::confirmation(tcPktType::discover));
      if(result) {
         char buffer[32];
         sprintf(buffer,"%u.%u.%u.%u", pkt.frame.data[2]&255, pkt.frame.data[3]&255,
                                       pkt.frame.data[4]&255, pkt.frame.data[5]&255);
         ip=std::string(buffer);
         mac=unpackMac(pkt.frame.smac);
      }
      return result;
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStreambuf::macDiscover() {
      static std::vector<discoveryResult> result;
      int retriesLeft = maxRetries;
      result.clear();
      if(isInterfaceOpen()) {
         discoveryResult discovery;
         while(retriesLeft>0 && result.size()<=0) {
            --retriesLeft;
            sendDiscoveryPacket();
            fetchPackets(timeout, timeout);
            while(receiveDiscoveryPacket(discovery.mac, discovery.ip)) {
               result.push_back(discovery);
               fetchPackets(timeout, timeout);
            }
         }
         if(result.size()>1) unique(result);
      }
      return result;
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStreambuf::udpDiscover(const std::string& ip) {
      static std::vector<discoveryResult> result;
      std::vector<tcStreambuf::tcStatus> udpDiscovery;
      std::vector<tcStreambuf::tcStatus>::iterator uIt;
      result.clear();
      udpDiscovery = readStatus(ip);
      discoveryResult discovery;
      for(uIt=udpDiscovery.begin(); uIt!=udpDiscovery.end(); ++uIt) {
         discovery.mac=uIt->getMacAddress();
         discovery.ip=uIt->getSourceIP();
         result.push_back(discovery);
      }
      if(result.size()>1) unique(result);
      return result;
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStreambuf::discover() {
      static std::vector<discoveryResult> result;
      std::vector<discoveryResult> macDiscovery;
      result.clear(); macDiscovery.clear();
      result=udpDiscover(); macDiscovery=macDiscover();
      result.insert(result.end(), macDiscovery.begin(), macDiscovery.end());
      if(result.size()>1) unique(result);
      return result;
   }

   void tcStreambuf::unique(std::vector<discoveryResult> &discoveries) {
      for(int first=0; first<discoveries.size(); ++first)
         for(int second=discoveries.size()-1; second>first; --second)
            if(discoveries[first].mac==discoveries[second].mac)
               discoveries.erase((discoveries.begin()+second));
   }

   const tcStreambuf::discoveryResult& tcStreambuf::setPeer(const discoveryResult& newPeer) {
      if(isConsoleSessionOpen()) closeSession();

      if(newPeer.mac.size()==0 || newPeer.ip.size()==0) {
         std::vector<tcStreambuf::discoveryResult> discovery;
         std::vector<tcStreambuf::discoveryResult>::iterator dIt;
         discovery = discover();
         for(dIt=discovery.begin(); dIt!=discovery.end() &&
                                    dIt->mac!=newPeer.mac &&
                                    dIt->ip!=newPeer.ip; ++dIt);
         if(dIt!=discovery.end()) {
            peer=(*dIt);
            peer.password=newPeer.password;
            return peer;
         }
      }

      char buffer[6]; memset(buffer, 0, sizeof(buffer));
      packMac(newPeer.mac, buffer);
      peer.mac=unpackMac(buffer);
      if(peer.mac=="00:00:00:00:00:00") peer.mac.clear();

      peer.ip=newPeer.ip;
      peer.password=newPeer.password;

      return peer;
   }

   const tcStreambuf::discoveryResult& tcStreambuf::setPeerByMac(const std::string& peerMac) {
      discoveryResult discovery;
      discovery.mac=peerMac;
      return setPeer(discovery);
   }

   const tcStreambuf::discoveryResult& tcStreambuf::setPeerByIp(const std::string& peerIp) {
      discoveryResult discovery;
      discovery.ip=peerIp;
      return setPeer(discovery);
   }

   void tcStreambuf::setPeerPassword(const std::string& password) {
      peer.password=password;
   }

   void tcStreambuf::sendOpenConsolePacket(const std::string& mac) {
      std::string data;
      data.clear(); data.push_back(tcPktType::startConsole);
      sendPacket(getIfaceMac(), mac, tcFrameType, data);
   }

   bool tcStreambuf::receiveOpenConsolePacket(const std::string& mac) {
      etherPacket pkt;
      int         pktLength;
      if(getNextPacketOfType(pkt, pktLength, tcPktType::confirmation(tcPktType::startConsole))) {
         return (unpackMac(pkt.frame.smac)==mac);
      } else return false;
   }

   bool tcStreambuf::openConsoleSession() {
      bool result = false;
      int retriesLeft = maxRetries;

      if(isInterfaceOpen()) {
         if(isConsoleSessionOpen()) closeSession();
         sendOpenConsolePacket(peer.mac);
         fetchPackets(timeout, 0);
         while(retriesLeft>0 && !(result=receiveOpenConsolePacket(peer.mac))) {
            --retriesLeft;
            sendOpenConsolePacket(peer.mac);
            fetchPackets(timeout, 0);
         }

         if(result) setTxDestMac(peer.mac);
      }
      consoleSessionIsOpen=result;
      return result;
   }

   bool tcStreambuf::isConsoleSessionOpen() {
      return consoleSessionIsOpen;
   }

   void tcStreambuf::sendStartMacRtsDumpPacket(const std::string& mac) {
      std::string data;
      data.clear(); data.push_back(tcPktType::startDump);
      sendPacket(getIfaceMac(), mac, tcFrameType, data);
   }

   bool tcStreambuf::receiveStartMacRtsDumpPacket(const std::string& mac) {
      etherPacket pkt;
      int         pktLength;
      if(getNextPacketOfType(pkt, pktLength, tcPktType::confirmation(tcPktType::startDump))) {
         return (unpackMac(pkt.frame.smac)==mac);
      } else return false;
   }

   bool tcStreambuf::startMacRtsDump() {
      bool result = false;
      int retriesLeft = maxRetries;

      if(!isMacRtsDumpRunning()) {
         if(isInterfaceOpen()) {
            sendStartMacRtsDumpPacket(peer.mac);
            fetchPackets(timeout, 0);
            while(retriesLeft>0 && !(result=receiveStartMacRtsDumpPacket(peer.mac))) {
               --retriesLeft;
               sendStartMacRtsDumpPacket(peer.mac);
               fetchPackets(timeout, 0);
            }

            if(result); // doSomething e.g. setTxDestMac(peer.mac);
         }
      } else
         result=true;

      macRtsDumpIsRunning=result;
      return result;
   }

   bool tcStreambuf::isMacRtsDumpRunning() {
      return macRtsDumpIsRunning;
   }

   void tcStreambuf::sendCloseSessionPacket(const std::string& mac) {
      std::string data;
      data.clear(); data.push_back(tcPktType::stopSession);
      sendPacket(getIfaceMac(), mac, tcFrameType, data);
   }

   bool tcStreambuf::receiveCloseSessionPacket(const std::string& mac) {
      etherPacket pkt;
      int         pktLength;
      if(getNextPacketOfType(pkt, pktLength, tcPktType::confirmation(tcPktType::stopSession))) {
         return (unpackMac(pkt.frame.smac)==mac);
      } else return false;
   }

   bool tcStreambuf::closeSession() {
      if(!isInterfaceOpen()) return true;
      if(!isConsoleSessionOpen() &&
         !isMacRtsDumpRunning()) return true;

      char buffer[6]; packMac(peer.mac, buffer);
      bool result = false;
      int retriesLeft = maxRetries;

      sendCloseSessionPacket(unpackMac(buffer));
      fetchPackets(timeout, 0);

      while(retriesLeft>0 && !(result=receiveCloseSessionPacket(unpackMac(buffer)))) {
         --retriesLeft;
         sendCloseSessionPacket(unpackMac(buffer));
         fetchPackets(timeout, 0);
      }

      macRtsDumpIsRunning=consoleSessionIsOpen=false;
      return result;
   }

   bool tcStreambuf::login(const std::string& password) {
      if(password.size()>0) setPeerPassword(password);

      bool consoleWasOpen;
      xmitMode oldTxMode, oldRxMode;

      if(!(consoleWasOpen=isConsoleSessionOpen())) {
         if(!openConsoleSession()) return false;
      }
      sync(); flush();
      oldTxMode=txMode; oldRxMode=rxMode;
      setRxMode(packetwise); setTxMode(packetwise);

      bool loginDone  = false;
      if(!isLoggedIn()) {
         int maxDataSize=(mtu-dhl-ethHeaderLen);
         std::string data;
         etherPacket pkt; int pktLength;
         int retriesLeft = maxRetries;

         while(retriesLeft>0 && !loginDone) {
            --retriesLeft;

            data=(peer.password+std::string("\x0d"));
            if(data.size()==xsputn(data.c_str(), data.size())) {
               fetchPackets(5000000, collectTimeout);  // Damn! What a slow device!
               memset(&pkt, 0, sizeof(pkt));
               while(getNextDataPacket(pkt, pktLength)) {
                  data=std::string(pkt.frame.data);
                  if(data.find("> ")==data.size()-2) loginDone=true;
                  fetchPackets(timeout, collectTimeout);
                  memset(&pkt, 0, sizeof(pkt));
               }
            }
         }
      } else loginDone=true;


      sync(); flush();
      setTxMode(oldTxMode); setRxMode(oldRxMode);
      if(!consoleWasOpen) closeSession();
      return loginDone;
   }

   bool tcStreambuf::isLoggedIn() {
      bool consoleWasOpen;
      xmitMode oldTxMode, oldRxMode;

      if(!(consoleWasOpen=isConsoleSessionOpen())) {
         if(!openConsoleSession()) return false;
      }
      sync(); flush();
      oldTxMode=txMode; oldRxMode=rxMode;
      setRxMode(packetwise); setTxMode(packetwise);

      int maxDataSize=(mtu-dhl-ethHeaderLen);
      bool isLoggedIn  = false,
           isLoggedOut = false;
      std::string data;
      etherPacket pkt; int pktLength;
      int retriesLeft = maxRetries;

      while(retriesLeft>0 && !isLoggedIn && !isLoggedOut) {
         --retriesLeft;

         data.clear();
         for(int i=0; i<maxDataSize; ++i) data.push_back(0x08);
         if(data.size()==xsputn(data.c_str(), data.size())) {
            data.clear();
            data.push_back(' '); data.push_back(0x0d);
            if(data.size()==xsputn(data.c_str(), data.size())) {
               fetchPackets(5000000, collectTimeout);  // Damn! What a slow device!
               memset(&pkt, 0, sizeof(pkt));
               while(getNextDataPacket(pkt, pktLength)) {
                  data=std::string(pkt.frame.data);
                  if(data.find("assword")!=std::string::npos) isLoggedOut=true;
                  if(data.find("> ")==data.size()-2) isLoggedIn=true;
                  fetchPackets(timeout, collectTimeout);
                  memset(&pkt, 0, sizeof(pkt));
               }
            }
         }
      }

      sync(); flush();

      setTxMode(oldTxMode); setRxMode(oldRxMode);
      if(!consoleWasOpen) closeSession();
      return isLoggedIn;
   }

   bool tcStreambuf::logout() {
      bool consoleWasOpen;
      xmitMode oldTxMode, oldRxMode;

      if(!(consoleWasOpen=isConsoleSessionOpen())) {
         if(!openConsoleSession()) return false;
      }
      sync(); flush();
      oldTxMode=txMode; oldRxMode=rxMode;
      setRxMode(packetwise); setTxMode(packetwise);

      bool logoutDone  = false;
      if(isLoggedIn()) {
         int maxDataSize=(mtu-dhl-ethHeaderLen);
         std::string data;
         etherPacket pkt; int pktLength;
         int retriesLeft = maxRetries;

         while(retriesLeft>0 && !logoutDone) {
            --retriesLeft;

            data=std::string("exit\x0d");
            if(data.size()==xsputn(data.c_str(), data.size())) {
               fetchPackets(1500000, collectTimeout);  // Damn! What a slow device!
               memset(&pkt, 0, sizeof(pkt));
               while(getNextDataPacket(pkt, pktLength)) {
                  data=std::string(pkt.frame.data);
                  if(data.find("assword")!=std::string::npos) logoutDone=true;
                  if(data.find("\x1b[2J")==0) logoutDone=true;
                  fetchPackets(timeout, collectTimeout);
                  memset(&pkt, 0, sizeof(pkt));
               }
            }
         }
      } else logoutDone=true;

      sync(); flush();
      setTxMode(oldTxMode); setRxMode(oldRxMode);
      if(!consoleWasOpen) closeSession();
      return logoutDone;
   }

   bool tcStreambuf::executeCommand(const std::string& command, std::string& result, const std::string& password) {
      bool consoleWasOpen;
      if(!(consoleWasOpen=isConsoleSessionOpen())) {
         if(!openConsoleSession()) return false;
      }

      xmitMode oldTxMode, oldRxMode;
      sync(); flush();
      oldTxMode=txMode; oldRxMode=rxMode;
      setRxMode(packetwise); setTxMode(packetwise);

      bool wasLoggedIn;
      if(!(wasLoggedIn=isLoggedIn())) {
         if(!login(password)) {
            if(!consoleWasOpen) closeSession();
            return false;
         }
      }

      bool anyDataReceived  = false;

      std::string data;
      etherPacket pkt; int pktLength;
      int retriesLeft = maxRetries;
      
      while(retriesLeft>0 && !anyDataReceived) {
         --retriesLeft;

         data=(command+std::string("\x0d"));
         if(data.size()==xsputn(data.c_str(), data.size())) {
            fetchPackets(1500000, collectTimeout);  // Damn! What a slow device!
            memset(&pkt, 0, sizeof(pkt));
            result.clear();
            while(getNextDataPacket(pkt, pktLength)) {
               data=std::string(pkt.frame.data);

               result.insert(result.end(), data.begin(), data.end());
               anyDataReceived=true;

               fetchPackets(timeout, collectTimeout);
               memset(&pkt, 0, sizeof(pkt));
            }
         }
      }

      if(anyDataReceived) {
         while(result[0]==0x0d || result[0]==0x0a)
            result.erase(result.begin());
         if(result[result.size()-2]=='>' && result[result.size()-1]==' ')
            result=result.substr(0, result.find_last_of("\x0a\x0d")+1);
      }

      if(!wasLoggedIn) logout();
      sync(); flush();
      setTxMode(oldTxMode); setRxMode(oldRxMode);
      if(!consoleWasOpen) closeSession();
      return anyDataReceived;
   }

   bool tcStreambuf::getNextPacketIt(std::vector<etherPacket>::iterator& pktIt,
                        std::vector<int>::iterator& sizeIt, int pktType)
   {
      int confType=-1;
      if(pktType<0) {
         confType=-pktType;
         pktType=tcPktType::confirm;
      }

      if(confType<0) 
         while( pktIt  != rxPacketsBuffer.end() &&
                sizeIt != rxPacketsSizes.end()  &&
                (pktIt->frame.data[0]&0xff)!=pktType)
                { ++pktIt; ++sizeIt; }
      else
         while( pktIt  != rxPacketsBuffer.end() &&
                sizeIt != rxPacketsSizes.end() &&
                ( (pktIt->frame.data[0]&0xff)!=pktType ||
                  (pktIt->frame.data[1]&0xff)!=confType) )
              { ++pktIt; ++sizeIt; }

      return pktIt  != rxPacketsBuffer.end() &&
             sizeIt != rxPacketsSizes.end();
   }

   bool tcStreambuf::getFirstPacketIt(std::vector<etherPacket>::iterator& pktIt,
                        std::vector<int>::iterator& sizeIt, int pktType)
   {
      pktIt   = rxPacketsBuffer.begin(),
      sizeIt  = rxPacketsSizes.begin();
      return getNextPacketIt(pktIt, sizeIt, pktType);
   }

   bool tcStreambuf::getNextPacketOfType(etherPacket& pkt,
                                         int& pktLength, int pktType)
   {
      std::vector<etherPacket>::iterator pktIt;
      std::vector<int>::iterator sizeIt;
      if(getFirstPacketIt(pktIt, sizeIt, pktType)) {
         pkt=(*pktIt); pktLength=(*sizeIt);
         rxPacketsBuffer.erase(pktIt);
         rxPacketsSizes.erase(sizeIt);
         return true;
      } else return false;
   }

   int tcStreambuf::peekNextPacketOfTypeSize(int pktType) {
      std::vector<etherPacket>::iterator pktIt;
      std::vector<int>::iterator sizeIt;
      if(getFirstPacketIt(pktIt, sizeIt, pktType))
         return (*sizeIt);
      else
         return 0;
   }

   bool tcStreambuf::getNextDataPacket(etherPacket& pkt, int& pktLength) {
      bool result = getNextPacketOfType(pkt, pktLength, tcPktType::recvConsole);
      if(result) {
         --pktLength;
         memcpy(pkt.frame.data, &pkt.frame.data[1], pktLength);
      }
      return result;
   }

   int tcStreambuf::peekNextDataPacketSize() {
      int result = peekNextPacketOfTypeSize(tcPktType::recvConsole);
      if(result>0) --result;
      return result;
   }

   std::streamsize tcStreambuf::showmanyc() {
      int bytesAvail;
      fetchPackets(0, 0);
      bytesRequested=0;

      if(rxPacketsSizes.size()==0) {
         fetchPackets(timeout, collectTimeout);
      }

      switch(rxMode) {
         case raw:
            bytesAvail=etherStreambuf::showmanyc();
            break;

         case packetwise:
            bytesAvail=peekNextDataPacketSize();
            if(bytesAvail>0) bytesAvail-=ethHeaderLen;
            break;

         case stream:
            {
               std::vector<etherPacket>::iterator pktIt;
               std::vector<int>::iterator         sizeIt;
               bytesAvail=0;
               if(getFirstPacketIt(pktIt, sizeIt, tcPktType::recvConsole)) {
                  bytesAvail=sizeof(recvPkt.frame.data);
                  realignBuffer(bytesAvail); bytesAvail=0;
                  do  {
                     bytesAvail+=((*sizeIt)-ethHeaderLen-1);
                     ++pktIt; ++sizeIt;
                  } while(getNextPacketIt(pktIt, sizeIt, tcPktType::recvConsole));

                  int bytesRead = gptr()-eback();
                  if(bytesAvail>(rxStreamBufSize-bytesRead))
                     bytesAvail=(rxStreamBufSize-bytesRead);
               }
            }
            break;
      }
      return bytesAvail;
   }

   void tcStreambuf::byteFix(tcStatusPacket &status) {
      status.unknown0x000=ntohs(status.unknown0x000);
      status.unknown0x002=ntohs(status.unknown0x002);
      sprintf(status.bootbaseFwVersion, "%s", trim(std::string(status.bootbaseFwVersion)).c_str());
      sprintf(status.rasFwVersion, "%s", trim(std::string(status.rasFwVersion)).c_str());
      sprintf(status.dslFwVersion, "%s", trim(std::string(status.dslFwVersion)).c_str());
      sprintf(status.lineStandard, "%s", trim(std::string(status.lineStandard)).c_str());
      sprintf(status.countryCode, "%s", trim(std::string(status.countryCode)).c_str());
      packMac(trim(std::string(status.macAddress)), status.macAddress);
      sprintf(status.macAddress, "%s", unpackMac(status.macAddress).c_str());
      sprintf(status.lineUptime, "%s", trim(std::string(status.lineUptime)).c_str());
      status.lineStatus=ntohs(status.lineStatus);
      status.unknown0x146=ntohs(status.unknown0x146);
      status.bandwidthDownInterleaved=ntohs(status.bandwidthDownInterleaved);
      status.bandwidthDownFastpath=ntohs(status.bandwidthDownFastpath);
      status.bandwidthUpInterleaved=ntohs(status.bandwidthUpInterleaved);
      status.bandwidthUpFastpath=ntohs(status.bandwidthUpFastpath);
      status.unknown0x150=ntohs(status.unknown0x150);
      status.unknown0x152=ntohs(status.unknown0x152);
      status.lineErrorsFecDownFastpath=ntohs(status.lineErrorsFecDownFastpath);
      status.lineErrorsFecDownInterleaved=ntohs(status.lineErrorsFecDownInterleaved);
      status.lineErrorsCrcDownFastpath=ntohs(status.lineErrorsCrcDownFastpath);
      status.lineErrorsCrcDownInterleaved=ntohs(status.lineErrorsCrcDownInterleaved);
      status.lineErrorsHecDownFastpath=ntohs(status.lineErrorsHecDownFastpath);
      status.lineErrorsHecDownInterleaved=ntohs(status.lineErrorsHecDownInterleaved);
      status.lineErrorsFecUpFastpath=ntohs(status.lineErrorsFecUpFastpath);
      status.lineErrorsFecUpInterleaved=ntohs(status.lineErrorsFecUpInterleaved);
      status.lineErrorsCrcUpFastpath=ntohs(status.lineErrorsCrcUpFastpath);
      status.lineErrorsCrcUpInterleaved=ntohs(status.lineErrorsCrcUpInterleaved);
      status.lineErrorsHecUpFastpath=ntohs(status.lineErrorsHecUpFastpath);
      status.lineErrorsHecUpInterleaved=ntohs(status.lineErrorsHecUpInterleaved);
      status.lineRelativeLoadDown=ntohs(status.lineRelativeLoadDown);
      status.lineNoiseMarginDown=ntohs(status.lineNoiseMarginDown);
      status.lineTxPowerUp=ntohs(status.lineTxPowerUp);
      status.lineAttenuationDown=ntohs(status.lineAttenuationDown);
      status.lineRelativeLoadUp=ntohs(status.lineRelativeLoadUp);
      status.lineNoisemarginUp=ntohs(status.lineNoisemarginUp);
      status.lineTxPowerDown=ntohs(status.lineTxPowerDown);
      status.lineAttenuationUp=ntohs(status.lineAttenuationUp);
      status.unknown0x27c=ntohs(status.unknown0x27c);
      status.unknown0x27e=ntohs(status.unknown0x27e);
      status.unknown0x280=ntohs(status.unknown0x280);
      status.unknown0x282=ntohs(status.unknown0x282);
      status.unknown0x284=ntohs(status.unknown0x284);
      status.unknown0x286=ntohs(status.unknown0x286);
      sprintf(status.hostname, "%s", trim(std::string(status.hostname)).c_str());
      sprintf(status.ipaddr, "%s", trim(std::string(status.ipaddr)).c_str());
      sprintf(status.netmask, "%s", trim(std::string(status.netmask)).c_str());
      status.vcQOS=ntohs(status.vcQOS);
      status.vcVPI=ntohs(status.vcVPI);
      status.vcVCI=ntohs(status.vcVCI);
      status.toneCount=ntohs(status.toneCount);
      status.unknown0x2f0=ntohs(status.unknown0x2f0);
      status.unknown0x2f2=ntohs(status.unknown0x2f2);
   }

   std::vector<tcStreambuf::tcStatus> tcStreambuf::readStatus(std::string ip) {
      std::vector<tcStatus> result; result.clear();

      if(ip.size()==0) {
         if(peer.ip.size()>0) {
            ip=peer.ip;
         } else {
            ip=std::string("255.255.255.255");
         }
      }

      struct hostent *host;
      host = gethostbyname(ip.c_str());
      if(host==NULL) return result;

      int udpsock = socket(AF_INET,SOCK_DGRAM,0);
      if(udpsock<0) return result;

      struct sockaddr_in dest, src, response;

      int enabled = 1;
      setsockopt(udpsock, SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled));  // continue anyway...

      src.sin_family = AF_INET; src.sin_addr.s_addr = htonl(INADDR_ANY);
      src.sin_port = htons(0);

      if(bind(udpsock, (struct sockaddr *) &src, sizeof(src))>=0) {
         dest.sin_family = host->h_addrtype;
         memcpy(&dest.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
         dest.sin_port = htons(tcFrameType);
         char buffer[2];
         buffer[0]=0; buffer[1]=1;
         int retriesLeft = maxRetries;

         tcStatusPacket status; memset(&status, 0 , sizeof(status));

         struct timeval *tv;
         fd_set fds;
         if(timeout>=0) tv=new struct timeval; else tv=NULL;
         do {
            --retriesLeft;
            if(sendto(udpsock, buffer, 2, 0, (struct sockaddr *)(&dest), sizeof(dest))>=0) {

               FD_ZERO(&fds); FD_SET(udpsock, &fds);
               if(tv!=NULL) { tv->tv_sec = 0; tv->tv_usec = timeout; }
               while(select(udpsock+1, &fds, NULL, NULL, tv)>0 && FD_ISSET(udpsock, &fds)) {
                  int       bytes=0;
                  socklen_t responseSize = sizeof(response);
                  if((sizeof(status)-sizeof(status.sourceIP))==recvfrom(udpsock, &status, sizeof(status), 0, (struct sockaddr*)(&response), &responseSize)) {
                     byteFix(status);
                     sprintf(status.sourceIP, "%s", inet_ntoa(response.sin_addr));
                     result.push_back(tcStatus(status));
                  }
               }
            }
         } while(retriesLeft>0 && result.size()==0);
      }
      close(udpsock);

      return result;
   }

   bool tcStreambuf::updatePeerStatus() {
      std::vector<tcStatus> statusObjs;
      statusObjs=readStatus(peer.ip);
      if(statusObjs.size()>0) {
         peerStatus=statusObjs[0];
      } else return false;
   }

   const tcStreambuf::tcStatus& tcStreambuf::getPeerStatus() {
      return peerStatus;
   }

   std::string tcStreambuf::trim(const std::string& str) {
      std::string::size_type first = str.find_first_not_of(" "),
                             last  = str.find_last_not_of(" "),
                             count = last-first+1;
      return str.substr(first, count);
   }

   tcStreambuf::tcStatus::tcStatus() {
      memset(&status, 0, sizeof(status));
      valid=false;
   }

   tcStreambuf::tcStatus::tcStatus(const tcStatusPacket& status) {
      this->status=status;
      valid=true;
   }

   int tcStreambuf::tcStatus::getUnknown0x000() const {
      return int(0xffff & status.unknown0x000);
   }

   int tcStreambuf::tcStatus::getUnknown0x002() const {
      return int(0xffff & status.unknown0x002);
   }

   std::string tcStreambuf::tcStatus::getBootbaseFwVersion() const {
      return std::string(status.bootbaseFwVersion);
   }

   std::string tcStreambuf::tcStatus::getRasFwVersion() const {
      return std::string(status.rasFwVersion);
   }

   std::string tcStreambuf::tcStatus::getDslFwVersion() const {
      return std::string(status.dslFwVersion);
   }

   std::string tcStreambuf::tcStatus::getLineStandard() const {
      return std::string(status.lineStandard);
   }

   std::string tcStreambuf::tcStatus::getCountryCode() const {
      return std::string(status.countryCode);
   }

   std::string tcStreambuf::tcStatus::getMacAddress() const {
      return std::string(status.macAddress);
   }

   std::string tcStreambuf::tcStatus::getLineUptime() const {
      return std::string(status.lineUptime);
   }

   int tcStreambuf::tcStatus::getLineStatus() const {
      return int(0xffff & status.lineStatus);
   }

   int tcStreambuf::tcStatus::getUnknown0x146() const {
      return int(0xffff & status.unknown0x146);
   }

   int tcStreambuf::tcStatus::getBandwidthDown() const {
      return getBandwidthDownInterleaved()+getBandwidthDownFastpath();
   }

   int tcStreambuf::tcStatus::getBandwidthUp() const {
      return getBandwidthUpInterleaved()+getBandwidthUpFastpath();
   }

   int tcStreambuf::tcStatus::getBandwidthDownMax() const {
      return getLineRelativeLoadDown()>0?(int)(100.0f*static_cast<float>(getBandwidthDown())/static_cast<float>(getLineRelativeLoadDown())):0;
   }

   int tcStreambuf::tcStatus::getBandwidthUpMax() const {
      return getLineRelativeLoadUp()>0?(int)(100.0f*static_cast<float>(getBandwidthUp())/static_cast<float>(getLineRelativeLoadUp())):0;
   }

   bool tcStreambuf::tcStatus::isInterleaved() const {
      return (getBandwidthDownInterleaved()+getBandwidthUpInterleaved())>0;
   }

   bool tcStreambuf::tcStatus::isFastpath() const {
      return (getBandwidthDownFastpath()+getBandwidthUpFastpath())>0;
   }

   int tcStreambuf::tcStatus::getBandwidthDownInterleaved() const {
      return int(0xffff & status.bandwidthDownInterleaved);
   }

   int tcStreambuf::tcStatus::getBandwidthDownFastpath() const {
      return int(0xffff & status.bandwidthDownFastpath);
   }

   int tcStreambuf::tcStatus::getBandwidthUpInterleaved() const {
      return int(0xffff & status.bandwidthUpInterleaved);
   }

   int tcStreambuf::tcStatus::getBandwidthUpFastpath() const {
      return int(0xffff & status.bandwidthUpFastpath);
   }

   int tcStreambuf::tcStatus::getUnknown0x150() const {
      return int(0xffff & status.unknown0x150);
   }

   int tcStreambuf::tcStatus::getUnknown0x152() const {
      return int(0xffff & status.unknown0x152);
   }

   int tcStreambuf::tcStatus::getLineErrorsFecDownFastpath() const {
      return int(0xffff & status.lineErrorsFecDownFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsFecDownInterleaved() const {
      return int(0xffff & status.lineErrorsFecDownInterleaved);
   }

   int tcStreambuf::tcStatus::getLineErrorsCrcDownFastpath() const {
      return int(0xffff & status.lineErrorsCrcDownFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsCrcDownInterleaved() const {
      return int(0xffff & status.lineErrorsCrcDownInterleaved);
   }

   int tcStreambuf::tcStatus::getLineErrorsHecDownFastpath() const {
      return int(0xffff & status.lineErrorsHecDownFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsHecDownInterleaved() const {
      return int(0xffff & status.lineErrorsHecDownInterleaved);
   }

   int tcStreambuf::tcStatus::getLineErrorsFecUpFastpath() const {
      return int(0xffff & status.lineErrorsFecUpFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsFecUpInterleaved() const {
      return int(0xffff & status.lineErrorsFecUpInterleaved);
   }

   int tcStreambuf::tcStatus::getLineErrorsCrcUpFastpath() const {
      return int(0xffff & status.lineErrorsCrcUpFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsCrcUpInterleaved() const {
      return int(0xffff & status.lineErrorsCrcUpInterleaved);
   }

   int tcStreambuf::tcStatus::getLineErrorsHecUpFastpath() const {
      return int(0xffff & status.lineErrorsHecUpFastpath);
   }

   int tcStreambuf::tcStatus::getLineErrorsHecUpInterleaved() const {
      return int(0xffff & status.lineErrorsHecUpInterleaved);
   }

   int tcStreambuf::tcStatus::getLineRelativeLoadDown() const {
      return int(0xffff & status.lineRelativeLoadDown);
   }

   float tcStreambuf::tcStatus::getLineNoiseMarginDown() const {
      return float(0xffff & status.lineNoiseMarginDown)/float(10);
   }

   float tcStreambuf::tcStatus::getLineTxPowerUp() const {
      return float(0xffff & status.lineTxPowerUp)/float(10);
   }

   float tcStreambuf::tcStatus::getLineAttenuationDown() const {
      return float(0xffff & status.lineAttenuationDown)/float(10);
   }

   int tcStreambuf::tcStatus::getLineRelativeLoadUp() const {
      return int(0xffff & status.lineRelativeLoadUp);
   }

   float tcStreambuf::tcStatus::getLineNoisemarginUp() const {
      return float(0xffff & status.lineNoisemarginUp)/float(10);
   }

   float tcStreambuf::tcStatus::getLineTxPowerDown() const {
      return float(0xffff & status.lineTxPowerDown)/float(10);
   }

   float tcStreambuf::tcStatus::getLineAttenuationUp() const {
      return float(0xffff & status.lineAttenuationUp)/float(10);
   }

   int tcStreambuf::tcStatus::getBitloadingValue(int tone) const {
      if(tone<0) return 0;
      if(tone>=getToneCount()) return 0;
      return (tone%2)?status.bitloadingValues[tone/2]/16:status.bitloadingValues[tone/2]%16;
   }

   int tcStreambuf::tcStatus::getUnknown0x27c() const {
      return int(0xffff & status.unknown0x27c);
   }

   int tcStreambuf::tcStatus::getUnknown0x27e() const {
      return int(0xffff & status.unknown0x27e);
   }

   int tcStreambuf::tcStatus::getUnknown0x280() const {
      return int(0xffff & status.unknown0x280);
   }

   int tcStreambuf::tcStatus::getUnknown0x282() const {
      return int(0xffff & status.unknown0x282);
   }

   int tcStreambuf::tcStatus::getUnknown0x284() const {
      return int(0xffff & status.unknown0x284);
   }

   int tcStreambuf::tcStatus::getUnknown0x286() const {
      return int(0xffff & status.unknown0x286);
   }

   std::string tcStreambuf::tcStatus::getHostname() const {
      return std::string(status.hostname);
   }

   std::string tcStreambuf::tcStatus::getIpaddr() const {
      return std::string(status.ipaddr);
   }

   std::string tcStreambuf::tcStatus::getNetmask() const {
      return std::string(status.netmask);
   }

   int tcStreambuf::tcStatus::getVcQOS() const {
      return int(0xffff & status.vcQOS);
   }

   int tcStreambuf::tcStatus::getVcVPI() const {
      return int(0xffff & status.vcVPI);
   }

   int tcStreambuf::tcStatus::getVcVCI() const {
      return int(0xffff & status.vcVCI);
   }

   int tcStreambuf::tcStatus::getToneCount() const {
      return int(0xffff & status.toneCount);
   }

   int tcStreambuf::tcStatus::getUnknown0x2f0() const {
      return int(0xffff & status.unknown0x2f0);
   }

   int tcStreambuf::tcStatus::getUnknown0x2f2() const {
      return int(0xffff & status.unknown0x2f2);
   }

   std::string tcStreambuf::tcStatus::getSourceIP() const {
      return std::string(status.sourceIP);
   }

   bool tcStreambuf::tcStatus::isValid() const {
      return valid;
   }
};
