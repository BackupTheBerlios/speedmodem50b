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
// $Id: etherStream.cpp,v 1.2 2006/12/04 16:33:55 miunske Exp $

#include "etherStream.h"

namespace tc {
   etherStream::etherStream() :std::iostream(static_cast<etherStreambuf*>(NULL)) {
      etherStreambuf* buf = static_cast<etherStreambuf*>(rdbuf());
      if(buf!=NULL) delete buf;
      buf=new etherStreambuf();
      rdbuf(buf);
   }

   etherStream::~etherStream() {
      etherStreambuf* buf = static_cast<etherStreambuf*>(rdbuf());
      rdbuf(NULL);
      delete buf;
   }

   bool etherStream::openInterface(std::string iface, long int frameType) {
      return static_cast<etherStreambuf*>(rdbuf())->openInterface(iface, frameType);
   }

   void etherStream::closeInterface() {
      static_cast<etherStreambuf*>(rdbuf())->closeInterface();
   }

   bool etherStream::isInterfaceOpen() {
      return static_cast<etherStreambuf*>(rdbuf())->isInterfaceOpen();
   }

   int  etherStream::getInterfaceFD() {
      return static_cast<etherStreambuf*>(rdbuf())->getInterfaceFD();
   }

   void etherStream::setRxTimeoutuSec(int usecs) {
      static_cast<etherStreambuf*>(rdbuf())->setRxTimeoutuSec(usecs);
   }

   bool etherStream::sendRawPacket(etherPacket& pkt, int pktLength) {
      return static_cast<etherStreambuf*>(rdbuf())->sendRawPacket(pkt, pktLength);
   }


   std::string etherStream::unpackMac(char* packedMac) {
      return static_cast<etherStreambuf*>(rdbuf())->unpackMac(packedMac);
   }

   void etherStream::packMac(const std::string& unpackedMac, char* packedMac) {
      static_cast<etherStreambuf*>(rdbuf())->packMac(unpackedMac, packedMac);
   }


   bool etherStream::isBroadcastMac(const std::string& mac) {
      return static_cast<etherStreambuf*>(rdbuf())->isBroadcastMac(mac);
   }

   bool etherStream::isMulticastMac(const std::string& mac) {
      return static_cast<etherStreambuf*>(rdbuf())->isMulticastMac(mac);
   }

   bool etherStream::isLocalMac(const std::string& mac) {
      return static_cast<etherStreambuf*>(rdbuf())->isLocalMac(mac);
   }

   bool etherStream::isUnicastMac(const std::string& mac) {
      return static_cast<etherStreambuf*>(rdbuf())->isUnicastMac(mac);
   }

   bool etherStream::isGlobalMac(const std::string& mac) {
      return static_cast<etherStreambuf*>(rdbuf())->isGlobalMac(mac);
   }

   std::string etherStream::getIfaceMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getIfaceMac();
   }

   std::string etherStream::getBroadcastMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getBroadcastMac();
   }

   std::string etherStream::getRxSourceMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getRxSourceMac();
   }

   std::string etherStream::getRxDestMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getRxDestMac();
   }

   std::string etherStream::getTxSourceMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getTxSourceMac();
   }

   std::string etherStream::getTxDestMac() {
      return static_cast<etherStreambuf*>(rdbuf())->getTxDestMac();
   }

   void etherStream::setTxSourceMac(const std::string& smac) {
      static_cast<etherStreambuf*>(rdbuf())->setTxSourceMac(smac);
   }

   void etherStream::setTxDestMac(const std::string& dmac) {
      static_cast<etherStreambuf*>(rdbuf())->setTxDestMac(dmac);
   }

   long int etherStream::getRxFrameType() {
      return static_cast<etherStreambuf*>(rdbuf())->getRxFrameType();
   }

   long int etherStream::getTxFrameType() {
      return static_cast<etherStreambuf*>(rdbuf())->getTxFrameType();
   }

   void etherStream::setTxFrameType(const long int& frameType) {
      static_cast<etherStreambuf*>(rdbuf())->setTxFrameType(frameType);
   }

   void etherStream::setTxMode(etherStreambuf::xmitMode txMode) {
      static_cast<etherStreambuf*>(rdbuf())->setTxMode(txMode);
   }

   void etherStream::setRxMode(etherStreambuf::xmitMode rxMode) {
      static_cast<etherStreambuf*>(rdbuf())->setRxMode(rxMode);
   }

   void etherStream::setMTU(int mtu) {
      static_cast<etherStreambuf*>(rdbuf())->setMTU(mtu);
   }

   void etherStream::setMaxDataLen(int mdl) {
      static_cast<etherStreambuf*>(rdbuf())->setMaxDataLen(mdl);
   }

   void etherStream::setDataHeader(std::string dhdr) {
      static_cast<etherStreambuf*>(rdbuf())->setDataHeader(dhdr);
   }

   void etherStream::setRxBufferSize(int size) {
      static_cast<etherStreambuf*>(rdbuf())->setRxBufferSize(size);
   }

   void etherStream::setRxTimeout(int ms) {
      static_cast<etherStreambuf*>(rdbuf())->setRxTimeout(ms);
   }

   void etherStream::setRxCollectTimeoutuSec(int usecs) {
      static_cast<etherStreambuf*>(rdbuf())->setRxCollectTimeoutuSec(usecs);
   }

   void etherStream::setRxCollectTimeout(int ms) {
      static_cast<etherStreambuf*>(rdbuf())->setRxCollectTimeout(ms);
   }
}
