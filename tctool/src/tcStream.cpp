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
// $Id: tcStream.cpp,v 1.1 2006/12/02 11:53:39 miunske Exp $

#include "tcStream.h"

namespace tc {
   tcStream::tcStream() : etherStream() {
      tcStreambuf* buf = static_cast<tcStreambuf*>(rdbuf());
      if(buf!=NULL) delete buf;
      buf=new tcStreambuf();
      rdbuf(buf);
   }

   bool tcStream::openInterface(const std::string& iface) {
      return static_cast<tcStreambuf*>(rdbuf())->openInterface(iface);
   }

   void tcStream::setMaxRetries(int retries) {
      static_cast<tcStreambuf*>(rdbuf())->setMaxRetries(retries);
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStream::macDiscover() {
      return static_cast<tcStreambuf*>(rdbuf())->macDiscover();
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStream::udpDiscover(const std::string& ip) {
      return static_cast<tcStreambuf*>(rdbuf())->udpDiscover(ip);
   }

   const std::vector<tcStreambuf::discoveryResult>& tcStream::discover() {
      return static_cast<tcStreambuf*>(rdbuf())->discover();
   }

   const tcStreambuf::discoveryResult& tcStream::setPeer(const tcStreambuf::discoveryResult& newPeer) {
      return static_cast<tcStreambuf*>(rdbuf())->setPeer(newPeer);
   }

   const tcStreambuf::discoveryResult& tcStream::setPeerByMac(const std::string& peerMac) {
      return static_cast<tcStreambuf*>(rdbuf())->setPeerByMac(peerMac);
   }

   const tcStreambuf::discoveryResult& tcStream::setPeerByIp(const std::string& peerIp) {
      return static_cast<tcStreambuf*>(rdbuf())->setPeerByIp(peerIp);
   }

   void tcStream::setPeerPassword(const std::string& password) {
      static_cast<tcStreambuf*>(rdbuf())->setPeerPassword(password);
   }

   bool tcStream::openConsoleSession() {
      return static_cast<tcStreambuf*>(rdbuf())->openConsoleSession();
   }

   bool tcStream::isConsoleSessionOpen() {
      return static_cast<tcStreambuf*>(rdbuf())->isConsoleSessionOpen();
   }

   bool tcStream::closeSession() {
      return static_cast<tcStreambuf*>(rdbuf())->closeSession();
   }

   bool tcStream::login(const std::string& password) {
      return static_cast<tcStreambuf*>(rdbuf())->login(password);
   }

   bool tcStream::isLoggedIn() {
      return static_cast<tcStreambuf*>(rdbuf())->isLoggedIn();
   }

   bool tcStream::logout() {
      return static_cast<tcStreambuf*>(rdbuf())->logout();
   }

   bool tcStream::executeCommand(const std::string& command, std::string& result, const std::string& password) {
      return static_cast<tcStreambuf*>(rdbuf())->executeCommand(command, result);
   }

   std::vector<tcStreambuf::tcStatus> tcStream::readStatus(const std::string& ip) {
      return static_cast<tcStreambuf*>(rdbuf())->readStatus(ip);
   }

   bool tcStream::updatePeerStatus() {
      return static_cast<tcStreambuf*>(rdbuf())->updatePeerStatus();
   }

   const tcStreambuf::tcStatus& tcStream::getPeerStatus() {
      return static_cast<tcStreambuf*>(rdbuf())->getPeerStatus();
   }
}
