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
// $Id: tcStream.h,v 1.4 2006/12/07 03:24:53 miunske Exp $

#ifndef _TCSTREAM_h_included_
#define _TCSTREAM_h_included_

#include "etherStream.h"
#include "tcStreambuf.h"
#include <string.h>

namespace tc {
   /** 
    * @class tcStream
    *
    * @brief This is an iostream using tcStreambuf to provide console-io to Tr*ndCh*p-devices via any ethernet-interface.
    *
    * This class provides a comfortable interface for ethernet-based console-io
    * with Tr*ndCh*p-devices using any ethernet-device.
    * It is mainly a wrapper-class using tcStreambuf which is derived from etherStreambuf.
    *
    * @see tcStreambuf
    * @see etherStream
    * @see etherStreambuf
    *
    * @author Ralf Miunske <miunske at users.berlios.de>
    *
    * $Header $
    */
   class tcStream : public etherStream {
      public:
         tcStream();
         bool openInterface(const std::string& iface);
         void setMaxRetries(int retries);
         const std::vector<tcStreambuf::discoveryResult>& macDiscover();
         const std::vector<tcStreambuf::discoveryResult>& udpDiscover(const std::string& ip = std::string());
         const std::vector<tcStreambuf::discoveryResult>& discover();
         const tcStreambuf::discoveryResult& setPeer(const tcStreambuf::discoveryResult& newPeer);
         const tcStreambuf::discoveryResult& setPeerByMac(const std::string& peerMac);
         const tcStreambuf::discoveryResult& setPeerByIp(const std::string& peerIp);
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
         std::vector<tcStreambuf::tcStatus> readStatus(const std::string& ip = std::string());
         bool updatePeerStatus();
         const tcStreambuf::tcStatus& getPeerStatus();
   };
};

#endif
