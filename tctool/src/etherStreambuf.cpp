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
// $Id: etherStreambuf.cpp,v 1.3 2006/12/06 17:48:25 miunske Exp $

#include "etherStreambuf.h"

#include <stdio.h>

namespace tc {
   etherStreambuf::etherStreambuf() {
      initValues();
   }

   etherStreambuf::~etherStreambuf() {
      if(sockfd>=0) closeInterface();
      if(rxStreamBuf!=NULL && rxStreamBufSize>0) delete[] rxStreamBuf;
   }

   bool etherStreambuf::openInterface(std::string iface,
                                      long int frameType) 
   {
      if(sockfd>=0) closeInterface();

      if(iface.length()==0) iface=this->iface;
      if(iface.length()==0) iface=std::string("eth0");
      if(frameType<0 && getTxFrameType()>0) frameType=getTxFrameType();
      if(frameType<0 && getRxFrameType()>0) frameType=getRxFrameType();
      if(frameType<0)                       frameType=0x000;

      sockfd = rp_openInterface(iface.c_str(), frameType&0xffff, ifmac);
      if(sockfd>=0) {
         this->iface=iface;
         setTxSourceMac(getIfaceMac());
         setTxFrameType(frameType);
      }
      return sockfd>=0;
   }

   void etherStreambuf::closeInterface() {
      if(sockfd<0) return;
      sync();
      close(sockfd);
      sockfd=-1;
   }

   bool etherStreambuf::isInterfaceOpen() {
      return sockfd>=0;
   }

   int etherStreambuf::getInterfaceFD() {
      return sockfd;
   }

   void etherStreambuf::setRxTimeoutuSec(int usecs) {
      timeout=usecs;
   }

   bool etherStreambuf::sendRawPacket(etherPacket& pkt, int pktLength) {
      if(sockfd<0) return false;
      return rp_sendPacket(iface.c_str(), sockfd, &pkt, pktLength)>=0;
   }

   bool etherStreambuf::sendPacket(const std::string& smac,
                                   const std::string& dmac,
                                   unsigned long int frameType,
                                   const std::string& data)
   {
      etherPacket pkt;
      int dataSize;

      dataSize=(data.length()+ethHeaderLen)<=mtu?data.length():mtu-ethHeaderLen;
      memset(&pkt, 0, sizeof(pkt));
      packMac(smac, pkt.frame.smac);
      packMac(dmac, pkt.frame.dmac);
      pkt.frame.type=htons(frameType&0xffff);
      memcpy(&pkt.frame.data, data.c_str(), dataSize);
      sendRawPacket(pkt, dataSize+ethHeaderLen);
   }

   bool etherStreambuf::sendDataPacket(etherPacket& pkt, int pktLength) {
      return sendRawPacket(pkt, pktLength);
   }

   std::string etherStreambuf::unpackMac(char* packedMac) {
      char unpackedMac[20];
      sprintf(unpackedMac, "%02X:%02X:%02X:%02X:%02X:%02X",
                  packedMac[0]&0xff, packedMac[1]&0xff, packedMac[2]&0xff, 
                  packedMac[3]&0xff, packedMac[4]&0xff, packedMac[5]&0xff);
      return std::string(unpackedMac);
   }

   void etherStreambuf::packMac(const std::string& unpackedMac, char* packedMac) {
      int m[6];
      bool copym;
      if(unpackedMac.length()>=6) {
         if(unpackedMac[0]=='x' && unpackedMac.length()>=7) {
            packMac(unpackedMac.substr(1), packedMac);
         } else if((unpackedMac[0]=='0' || unpackedMac[0]==92) && unpackedMac[1]=='x' && unpackedMac.length()>=8) {
            packMac(unpackedMac.substr(2), packedMac);
         } else {
            copym=         (sscanf(unpackedMac.c_str(), "%x:%x:%x:%x:%x:%x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5])==6);
            copym=copym || (sscanf(unpackedMac.c_str(), "%x-%x-%x-%x-%x-%x", &m[0], &m[1], &m[2], &m[3], &m[4], &m[5])==6);
            if(copym)
               for(int i=0; i<6; ++i) packedMac[i]=m[i]&0xff;
            else
               for(int i=0; i<6; ++i) packedMac[i]=unpackedMac[i];
         } 
      }
   }

   bool etherStreambuf::isBroadcastMac(const std::string& mac) {
      char m[6]; packMac(mac, m);
      return (m[0]&m[1]&m[2]&m[3]&m[4]&m[5])==0xff;
   }

   bool etherStreambuf::isMulticastMac(const std::string& mac) {
      char m[6]; packMac(mac, m);
      return (m[0]&1>0);
   }

   bool etherStreambuf::isLocalMac(const std::string& mac) {
      char m[6]; packMac(mac, m);
      return (m[0]&2>0);
   }

   bool etherStreambuf::isUnicastMac(const std::string& mac) {
      return ! isMulticastMac(mac);
   }

   bool etherStreambuf::isGlobalMac(const std::string& mac) {
      return ! isLocalMac(mac);
   }
   std::string etherStreambuf::getIfaceMac() {
      return unpackMac(ifmac);
   }

   std::string etherStreambuf::getBroadcastMac() {
      return std::string("FF:FF:FF:FF:FF:FF");
   }

   std::string etherStreambuf::getRxSourceMac() {
      return unpackMac(recvPkt.frame.smac);
   }

   std::string etherStreambuf::getRxDestMac() {
      return unpackMac(recvPkt.frame.dmac);
   }

   std::string etherStreambuf::getTxSourceMac() {
      return unpackMac(sendPkt.frame.smac);
   }

   std::string etherStreambuf::getTxDestMac() {
      return unpackMac(sendPkt.frame.dmac);
   }

   void etherStreambuf::setTxSourceMac(const std::string& smac) {
      packMac(smac, sendPkt.frame.smac);
   }

   void etherStreambuf::setTxDestMac(const std::string& dmac) {
      packMac(dmac, sendPkt.frame.dmac);
   }

   long int etherStreambuf::getRxFrameType() {
      return ntohs(recvPkt.frame.type&0xffff);
   }

   long int etherStreambuf::getTxFrameType() {
      return ntohs(sendPkt.frame.type&0xffff);
   }

   void etherStreambuf::setTxFrameType(const long int& frameType) {
      sendPkt.frame.type=htons(frameType&0xffff);
   }

   void etherStreambuf::setMaxDataLen(int mdl) {
      mdl=(mdl+dhl+ethHeaderLen<=sizeof(sendPkt))?mdl:sizeof(sendPkt)-dhl+ethHeaderLen;
      setMTU(mdl+dhl+ethHeaderLen);
   }

   void etherStreambuf::setMTU(int mtu) {
      if(mtu>dhl+ethHeaderLen)  setTxBuffer(dhl, mtu);
      else if(mtu>ethHeaderLen) setTxBuffer(0, mtu);
      else setTxBuffer(0, sizeof(sendPkt));
   }

   void etherStreambuf::setDataHeader(std::string dhdr) {
      int i, dataLen = mtu-(ethHeaderLen+dhl),
             newdhl = dhdr.length(),
             newmtu  = ethHeaderLen+newdhl+dataLen;
      if(newmtu>sizeof(sendPkt)) {
         newmtu  = sizeof(sendPkt);
         if(newmtu-(ethHeaderLen+newdhl)<1) {
            newdhl = 0;
            newmtu = ethHeaderLen+dataLen;
         }
      }
      setTxBuffer(newdhl, newmtu);
      for(i=0; i<newdhl; ++i) sendPkt.frame.data[i]=dhdr[i];
   }

   void etherStreambuf::setTxBuffer(int dhl, int mtu) {
      if(mtu<=sizeof(sendPkt)) {
         int streamDataLength = mtu - (ethHeaderLen+dhl);

         if(streamDataLength>0) {
            sync();
            setp(&(sendPkt.frame.data[dhl]),
                 &(sendPkt.frame.data[mtu-ethHeaderLen-1]));
            this->dhl=dhl;
            this->mtu=mtu;
         }
      }
   }

   void etherStreambuf::setTxMode(xmitMode txMode) {
      sync();
      this->txMode=txMode;
   }

   void etherStreambuf::setRxMode(xmitMode rxMode) {
      this->rxMode=rxMode;
      switch(rxMode) {
         case raw:
         case packetwise:
            setRxBufferSize(0);
            break;
         case stream:
            setRxBufferSize(rxStreamBufDefSize);
            break;
      }
   }

   void etherStreambuf::setRxBufferSize(int size) {
      char* oldBuffer;
      char* newBuffer;
      int   oldBufferBytesRead, oldBufferBytesUnread,
            newBufferBytesRead, newBufferBytesUnread,
            newBufferBytesFree;

      oldBuffer            = rxStreamBuf;

      if(size>ethHeaderLen) {
         newBuffer = new char[size];
         memset(newBuffer, 0x00, sizeof(size));
         if(eback()==oldBuffer) {
            oldBufferBytesRead   = gptr()-eback(),
            oldBufferBytesUnread = egptr()-gptr(),
            newBufferBytesFree   = size-oldBufferBytesUnread,
            newBufferBytesUnread = oldBufferBytesUnread+(newBufferBytesFree<0)?newBufferBytesFree:0,
            newBufferBytesRead   = (newBufferBytesFree>oldBufferBytesRead)?oldBufferBytesRead:newBufferBytesFree;
            if(newBufferBytesFree<0) newBufferBytesFree=0;
            if(newBufferBytesRead<0) newBufferBytesRead=0;

            for(int p=-newBufferBytesRead; p<newBufferBytesUnread; ++p)
               newBuffer[newBufferBytesRead+p] = oldBuffer[oldBufferBytesRead+p];
         } else {
            newBufferBytesRead=newBufferBytesUnread=0;
         }
      } else {
         size=0;
         newBufferBytesUnread=0;
         newBufferBytesRead=0;
         if(rxMode==raw) {
            newBuffer=recvPkt.raw;
         } else {
            newBuffer=recvPkt.frame.data;
            if(rxMode==stream) setRxMode(packetwise);
         }
      }

      setg(newBuffer, &newBuffer[newBufferBytesRead], &newBuffer[newBufferBytesRead+newBufferBytesUnread]);
      if(oldBuffer!=NULL && rxStreamBufSize>0) delete[] oldBuffer;
      rxStreamBuf=newBuffer;
      rxStreamBufSize=size; rxStreamBufDefSize=size>0?size:rxStreamBufDefSize;
   }

   void etherStreambuf::setRxTimeout(int ms) {
      timeout=ms*1000;
   }

   void etherStreambuf::setRxCollectTimeoutuSec(int usecs) {
      collectTimeout=usecs;
   }

   void etherStreambuf::setRxCollectTimeout(int ms) {
      setRxCollectTimeoutuSec(ms*1000);
   }

   bool etherStreambuf::receiveRawPacket(etherPacket& pkt, int& pktLength) {
      if(sockfd<0) return false;
      return rp_receivePacket(sockfd, &pkt, &pktLength)==0;
   }

   void etherStreambuf::fetchPackets(int timeout, int collectTimeout) {
      if(sockfd<0) return;

      int retval;
      struct timeval *tv;
      fd_set fds;

      if(timeout>=0)
         tv=new struct timeval;
      else
         tv=NULL;

      FD_ZERO(&fds); FD_SET(sockfd, &fds);
      if(tv!=NULL) { tv->tv_sec = 0; tv->tv_usec = timeout; }
      while(select(sockfd+1, &fds, NULL, NULL, tv)>0) {
         if(FD_ISSET(sockfd, &fds)) {
            if(receiveRawPacket(fetchPkt, fetchPktSize)) {
               rxPacketsBuffer.push_back(fetchPkt);
               rxPacketsSizes.push_back(fetchPktSize);
            }
         }
         FD_ZERO(&fds); FD_SET(sockfd, &fds);
         if(tv!=NULL) { tv->tv_sec = 0; tv->tv_usec = collectTimeout; }
      }
      return;
   }

   bool etherStreambuf::getNextDataPacket(etherPacket& pkt, int& pktLength) {
      if(rxPacketsSizes.size()>0) {
         pkt=rxPacketsBuffer[0];
         pktLength=rxPacketsSizes[0];
         rxPacketsBuffer.erase(rxPacketsBuffer.begin());
         rxPacketsSizes.erase(rxPacketsSizes.begin());
         return true;
      } else return false;
   }

   int etherStreambuf::peekNextDataPacketSize() {
      if(rxPacketsSizes.size()>0)
         return rxPacketsSizes[0];
      else
         return 0;
   }

   void etherStreambuf::initValues() {
      ethHeaderLen = sizeof(sendPkt.frame.dmac)+sizeof(sendPkt.frame.smac)+sizeof(sendPkt.frame.type);
      sockfd=-1;
      collectTimeout=timeout=0;
      memset(&sendPkt, 0x00, sizeof(sendPkt)); recvPktSize=0;
      memset(&recvPkt, 0x00, sizeof(recvPkt));
      bytesRequested=0;
      rxStreamBuf=NULL;
      rxStreamBufSize=0;
      rxStreamBufDefSize=50*sizeof(recvPkt.frame.data);
      setTxMode(packetwise);
      setRxMode(packetwise);
      setRxTimeoutuSec(10000);
      rxPacketsBuffer.clear();
      rxPacketsSizes.clear();
      setTxBuffer(0, sizeof(sendPkt));
      setTxDestMac(getBroadcastMac());
      setTxFrameType(0x0000);
      setMTU(sizeof(sendPkt));
   }

   std::streamsize etherStreambuf::xsputn(const char_type *s,
                                          std::streamsize n)
   {
      std::streamsize realn, rc;
      switch(txMode) {
         case raw:
            realn = n>mtu?mtu:n;
            rc = std::streambuf::xsputn(s, realn);
            memcpy(sendPkt.raw, s, realn);
            if(sendRawPacket(sendPkt, realn)) return n;
               else return 0;
            break;
         case packetwise:
            realn = n>mtu-(ethHeaderLen+dhl)?mtu-(ethHeaderLen+dhl):n;
            rc = std::streambuf::xsputn(s, realn);
            if(rc>0) sync();
            if(rc>=realn) return n;
               else return rc;
            break;
         case stream:
            return std::streambuf::xsputn(s, n);
            break;
      }
   }

   etherStreambuf::int_type etherStreambuf::overflow(int_type c) {
      if(!traits_type::eq_int_type(traits_type::eof(), c)) {
         traits_type::assign(*pptr(), traits_type::to_char_type(c));
         pbump(1);
      }
      return sync()==0?traits_type::not_eof(c):traits_type::eof();
   }

   int etherStreambuf::sync() {
      if(pbase()==pptr()) {
         return 0;
      } else {
         if(sendDataPacket(sendPkt, pptr()-sendPkt.raw)) {
            pbump(pbase()-pptr());
            return 0;
         } else return -1;
      }
   }

   std::streamsize etherStreambuf::xsgetn(char_type *s,
                                          std::streamsize n)
   {
      std::streamsize rc;
      fetchPackets(0, 0);
      if(bytesRequested==0) bytesRequested=n;

      rc = std::streambuf::xsgetn(s, n);
      return rc;
   }

   void etherStreambuf::realignBuffer(int& bytesRequested) {
      int bytesRead   = gptr()-eback(),
          bytesUnread = egptr()-gptr();
      if(rxStreamBufSize>0 && eback()==rxStreamBuf && rxStreamBuf!=NULL) {
         if( bytesRead>(rxStreamBufSize/10) ||
            (bytesRead+bytesUnread+bytesRequested)>rxStreamBufSize) {
            int newBytesRead = rxStreamBufSize/10;
            if(newBytesRead>bytesRead) newBytesRead=bytesRead;
            if((newBytesRead+bytesUnread+bytesRequested)>rxStreamBufSize) {
               newBytesRead-=((newBytesRead+bytesUnread+bytesRequested)-rxStreamBufSize);
               if(newBytesRead<0) {
                  bytesRequested+=newBytesRead;
                  newBytesRead=0;
               }
            }
            int bytesReadDiff = bytesRead-newBytesRead;
            if(bytesReadDiff>0) {
               memcpy(eback(), eback()+bytesReadDiff, egptr()-eback()-(bytesReadDiff-1));
               bytesRead=newBytesRead;
            }
            setg(eback(), eback()+bytesRead, eback()+bytesRead+bytesUnread);
         }
      }
   }

   std::streamsize etherStreambuf::showmanyc() {
      int bytesAvail;
      fetchPackets(0, 0);
      bytesRequested=0;

      if(rxPacketsSizes.size()==0) {
         fetchPackets(timeout, collectTimeout);
      }

      switch(rxMode) {
         case raw:
            bytesAvail=rxPacketsSizes.size()>0?rxPacketsSizes[0]:0;
            break;

         case packetwise:
            bytesAvail=rxPacketsSizes.size()>0?rxPacketsSizes[0]-ethHeaderLen:0;
            break;

         case stream:
            bytesAvail=sizeof(recvPkt.frame.data);
            realignBuffer(bytesAvail);
            bytesAvail=0;
            for(std::vector<int>::iterator sIt=rxPacketsSizes.begin();
                sIt!=rxPacketsSizes.end(); ++sIt) bytesAvail+=((*sIt)-ethHeaderLen);
            {
               int bytesRead = gptr()-eback();
               if(bytesAvail>(rxStreamBufSize-bytesRead)) bytesAvail=(rxStreamBufSize-bytesRead);
            }
            break;
      }
      return bytesAvail;
   }

   etherStreambuf::int_type etherStreambuf::underflow() {
      switch(rxMode) {
         case raw:
            getNextDataPacket(recvPkt, recvPktSize);
            setg(recvPkt.raw, recvPkt.raw, &recvPkt.raw[recvPktSize]);
            break;

         case packetwise:
            getNextDataPacket(recvPkt, recvPktSize);
            setg(recvPkt.frame.data, recvPkt.frame.data, &recvPkt.frame.data[recvPktSize-ethHeaderLen]);
            break;

         case stream:
            if(peekNextDataPacketSize()>0 && bytesRequested>0) {
               realignBuffer(bytesRequested);
               int  bytesToRead = bytesRequested,
                    bytesFree   = &eback()[rxStreamBufSize]-gptr();

               if(bytesToRead>bytesFree) bytesToRead=bytesFree;

               if(bytesToRead>0 && bytesToRead<(peekNextDataPacketSize()-ethHeaderLen) && (peekNextDataPacketSize()-ethHeaderLen)<=bytesFree) {
                  getNextDataPacket(recvPkt, recvPktSize);
                  memcpy(egptr(), recvPkt.frame.data, recvPktSize-ethHeaderLen);
                  setg(eback(), gptr(), egptr()+recvPktSize-ethHeaderLen);
                  bytesToRead=0;
                  bytesFree-=(recvPktSize-ethHeaderLen);
               } else
               while(bytesToRead>0 && bytesToRead>=(peekNextDataPacketSize()-ethHeaderLen) && (peekNextDataPacketSize()-ethHeaderLen)<=bytesFree && peekNextDataPacketSize()>0) {
                  getNextDataPacket(recvPkt, recvPktSize);
                  memcpy(egptr(), recvPkt.frame.data, recvPktSize-ethHeaderLen);
                  setg(eback(), gptr(), egptr()+recvPktSize-ethHeaderLen);
                  bytesToRead=bytesToRead-(recvPktSize-ethHeaderLen);
                  bytesFree-=(recvPktSize-ethHeaderLen);
               }
               bytesRequested=0;
            }
            break;
      }

      if(gptr()<egptr())
         return traits_type::to_int_type(*gptr());
      else
         return traits_type::eof();
   }

   void etherStreambuf::flush() {
      etherPacket pkt; int pktLength;
      fetchPackets(0,0);
      while(getNextDataPacket(pkt, pktLength));
      gbump(egptr()-gptr());
   }
};
