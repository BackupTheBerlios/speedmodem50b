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
// $Id: tcRtsDumpbuf.h,v 1.1 2006/12/07 03:25:28 miunske Exp $

#ifndef _TCRTSDUMPBUF_h_included_
#define _TCRTSDUMPBUF_h_included_

#include "etherStreambuf.h"
#include "interface.h"
#include <string.h>
#include <vector>

namespace tc {
   typedef union {
      char raw[_ETH_DATA_LEN_+14];
      struct {
         char        dmac[6];
         char        smac[6];
         UINT16_t    type;
         char        frameNo;
         char        hSize;      // due to word-alignment :-/
         char        lSize;
         char        data[_ETH_DATA_LEN_-3];
         UINT16_t    size;
      } frame;
   } tcRtsPacket;

   typedef union {
      char     bRaw[36];
      UINT16_t wRaw[18];
      struct {
         char        dummy;
      } frame;
   } tcRtsFrame;

   /**
    * @class tcRtsDumpbuf
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
   class tcRtsDumpbuf : public etherStreambuf {
      public:
         static const unsigned long int tcFrameType  = 0xaa7f;

      protected:
         std::string peerMac;
         std::string rtsData;

      public:
         tcRtsDumpbuf(const std::string& iface, const std::string& peer);
         bool doSomething();

      protected:
         virtual void initValues();
         bool openInterface(const std::string& iface);
         virtual void closeInterface();
         bool getNextRtsFrame(tcRtsFrame& frame);
         void collectPackets();
         virtual bool sendRawPacket(etherPacket& pkt, int pktLength);
         virtual bool receiveRawPacket(etherPacket& pkt, int& pktLength);
         bool getNextPacketIt(std::vector<etherPacket>::iterator& pktIt,
                              std::vector<int>::iterator& sizeIt);
         bool getFirstPacketIt(std::vector<etherPacket>::iterator& pktIt,
                              std::vector<int>::iterator& sizeIt);
         bool getNextPacket(etherPacket& pkt, int& pktLength);
         int  peekNextPacketSize();
         bool getNextRtsPacket(tcRtsPacket& pkt, int& pktLength);
         virtual bool getNextDataPacket(etherPacket& pkt, int& pktLength);
         virtual int  peekNextDataPacketSize();
         virtual std::streamsize showmanyc();
         void byteFix(tcRtsPacket& pkt);
         void byteFix(tcRtsFrame& frame);
   };
};

#endif
