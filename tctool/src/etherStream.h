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
// $Id: etherStream.h,v 1.2 2006/12/06 17:48:25 miunske Exp $

#ifndef _ETHERSTREAM_h_included_
#define _ETHERSTREAM_h_included_

#include "etherStreambuf.h"
#include <iostream>
#include <string.h>

namespace tc {
   /** 
    * @class etherStream
    *
    * @brief This is an iostream using etherStreambuf to provide direct io via any ethernet-interface.
    *
    * This class provides a comfortable interface for direct packet-io using any ethernet-device.
    * It is mainly a wrapper-class using etherStreambuf.
    *
    * @see etherStreambuf
    *
    * @author Ralf Miunske <miunske at users.berlios.de>
    *
    * $Header $
    */
   class etherStream : public std::iostream {
      public:
         etherStream();
         virtual ~etherStream();

         bool openInterface(std::string iface, long int frameType);
         void closeInterface();
         bool isInterfaceOpen();
         int  getInterfaceFD();
         void setRxTimeoutuSec(int usecs);

         bool sendRawPacket(etherPacket& pkt, int pktLength);

         std::string unpackMac(char* packedMac);
         void packMac(const std::string& unpackedMac, char* packedMac);

         bool isBroadcastMac(const std::string& mac);
         bool isMulticastMac(const std::string& mac);
         bool isLocalMac(const std::string& mac);
         bool isUnicastMac(const std::string& mac);
         bool isGlobalMac(const std::string& mac);

         std::string getIfaceMac();
         std::string getBroadcastMac();
         std::string getRxSourceMac();
         std::string getRxDestMac();
         std::string getTxSourceMac();
         std::string getTxDestMac();
         void setTxSourceMac(const std::string& smac);
         void setTxDestMac(const std::string& dmac);
         long int getRxFrameType();
         long int getTxFrameType();
         void setTxFrameType(const long int& frameType);
         void setTxMode(etherStreambuf::xmitMode txMode);
         void setRxMode(etherStreambuf::xmitMode rxMode);
         void setMTU(int mtu);
         void setMaxDataLen(int mdl);
         void setDataHeader(std::string dhdr);
         void setRxBufferSize(int size);
         void setRxTimeout(int ms);
         void setRxCollectTimeoutuSec(int usecs);
         void setRxCollectTimeout(int ms);
   };
};

#endif
