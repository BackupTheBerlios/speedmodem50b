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
// $Id: tcRtsDumpbuf.cpp,v 1.2 2006/12/07 14:53:47 miunske Exp $

#include "tcRtsDumpbuf.h"

namespace tc {
   tcRtsDumpbuf::tcRtsDumpbuf(const std::string& iface, const std::string& peer)
   {
      initValues();
      if(openInterface(iface)) peerMac=peer;
   }

   void tcRtsDumpbuf::initValues() {
      etherStreambuf::initValues();
      setTxMode(packetwise);
      setRxMode(packetwise);
      setRxTimeoutuSec(1100000);
      setRxCollectTimeoutuSec(0);
      setTxFrameType(tcFrameType);
      peerMac=std::string();
      rtsData=std::string();
   }

   bool tcRtsDumpbuf::openInterface(const std::string& iface)  {
      return etherStreambuf::openInterface(iface, tcFrameType);
   }

   void tcRtsDumpbuf::closeInterface()  { etherStreambuf::closeInterface(); }

   bool tcRtsDumpbuf::doSomething() {
      tcRtsFrame frame;
      bool result = false;
      while(getNextRtsFrame(frame)) {
         result=true;
         printf(" %04x",ntohs(frame.wRaw[0]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[1]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[2]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[3]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[4]&0xffff));
         printf(" %3u",frame.bRaw[10]&0xff);
         printf(" %3u",frame.bRaw[11]&0xff);
         printf(" %04x",ntohs(frame.wRaw[6]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[7]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[8]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[9]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[10]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[11]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[12]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[13]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[14]&0xffff));
         printf(" %04x",ntohs(frame.wRaw[15]&0xffff));
         printf("\n");
      }
      return result;
   }

   bool tcRtsDumpbuf::getNextRtsFrame(tcRtsFrame& frame) {
      collectPackets(0, 0);
      if(rtsData.size()<sizeof(frame)) {
         collectPackets(timeout, collectTimeout);
         if(rtsData.size()<sizeof(frame)) return false;
      }
      memcpy(frame.bRaw, rtsData.c_str(), sizeof(frame));
      rtsData.erase(rtsData.begin(), rtsData.begin()+sizeof(frame));
      //byteFix(frame);
      return true;
   }

   void tcRtsDumpbuf::collectPackets(int timeout, int collectTimeout) {
      tcRtsPacket pkt;
      int         pktLength;
      fetchPackets(timeout, collectTimeout);
      while(getNextRtsPacket(pkt, pktLength))
         for(int adr=0; adr<pkt.frame.size; ++adr)
            rtsData.push_back(pkt.frame.data[adr]);
   }

   bool tcRtsDumpbuf::receiveRawPacket(etherPacket& pkt, int& pktLength) {
      if(etherStreambuf::receiveRawPacket(pkt, pktLength)) {
         if(peerMac!=unpackMac(pkt.frame.smac)) return false;
         return true;
      } else return false;
   }

   bool tcRtsDumpbuf::getNextPacketIt(std::vector<etherPacket>::iterator& pktIt, std::vector<int>::iterator& sizeIt)
   {
      return pktIt  != rxPacketsBuffer.end() &&
             sizeIt != rxPacketsSizes.end();
   }

   bool tcRtsDumpbuf::getFirstPacketIt(std::vector<etherPacket>::iterator& pktIt,
                        std::vector<int>::iterator& sizeIt)
   {
      pktIt   = rxPacketsBuffer.begin(),
      sizeIt  = rxPacketsSizes.begin();
      return getNextPacketIt(pktIt, sizeIt);
   }

   bool tcRtsDumpbuf::getNextPacket(etherPacket& pkt, int& pktLength)
   {
      std::vector<etherPacket>::iterator pktIt;
      std::vector<int>::iterator sizeIt;
      if(getFirstPacketIt(pktIt, sizeIt)) {
         pkt=(*pktIt); pktLength=(*sizeIt);
         rxPacketsBuffer.erase(pktIt);
         rxPacketsSizes.erase(sizeIt);
         return true;
      } else return false;
   }

   int tcRtsDumpbuf::peekNextPacketSize() {
      std::vector<etherPacket>::iterator pktIt;
      std::vector<int>::iterator sizeIt;
      if(getFirstPacketIt(pktIt, sizeIt))
         return (*sizeIt);
      else
         return 0;
   }

   bool tcRtsDumpbuf::getNextRtsPacket(tcRtsPacket& pkt, int& pktLength) {
      etherPacket* ePkt;
      ePkt=(etherPacket*)(&pkt);
      bool result = getNextPacket(*ePkt, pktLength);
      if(result) byteFix(pkt);
      return result;
   }

   void tcRtsDumpbuf::byteFix(tcRtsPacket& pkt) {
      pkt.frame.size=((0xff&pkt.frame.hSize)<<8)+(0xff&pkt.frame.lSize);
   }

   void tcRtsDumpbuf::byteFix(tcRtsFrame& frame) {
      for(int i=0; i<sizeof(frame)/2; ++i)
         frame.wRaw[i]=ntohs(frame.wRaw[i]);
   }

   // disable streaming capabilities
   bool tcRtsDumpbuf::sendRawPacket(etherPacket& pkt, int pktLength) { return false;}
   bool tcRtsDumpbuf::getNextDataPacket(etherPacket& pkt, int& pktLength) { return false; }
   int  tcRtsDumpbuf::peekNextDataPacketSize() { return 0; }
   std::streamsize tcRtsDumpbuf::showmanyc() { return 0; }
};
