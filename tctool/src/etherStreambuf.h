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
// $Id: etherStreambuf.h,v 1.4 2006/12/07 03:24:53 miunske Exp $

#ifndef _ETHERSTREAMBUF_h_included_
#define _ETHERSTREAMBUF_h_included_

#include "interface.h"
#include <streambuf>
#include <vector>
#include <string.h>

#define _ETH_DATA_LEN_ 1500

namespace tc {
   typedef union {
      char raw[_ETH_DATA_LEN_+14];
      struct {
         char        dmac[6];
         char        smac[6];
         UINT16_t    type;
         char        data[_ETH_DATA_LEN_];
      } frame;
   } etherPacket;

   /** 
    * @class etherStreambuf
    *
    * @brief This is a streambuf to provide direct io via any ethernet-interface.
    *
    * This class provides a comfortable interface for direct packet-io using any ethernet-device.
    * It should always be used by it's wrapper-class etherStream.
    *
    * @see etherStream
    * 
    * @author Ralf Miunske <miunske at users.berlios.de>
    *
    * $Header $
    */
   class etherStreambuf : public std::streambuf {
      public:
         typedef enum { raw,
                        packetwise,
                        stream
                      } xmitMode;

      protected:
         std::string    iface;
         char           ifmac[6];
         int            sockfd;
         int            timeout;
         int            collectTimeout;
         int            mtu, dhl;
         xmitMode       txMode;
         xmitMode       rxMode;

         etherPacket	          sendPkt,
                                  recvPkt,
                                  fetchPkt;
         int                      recvPktSize,
                                  fetchPktSize;
         std::vector<etherPacket> rxPacketsBuffer;
         std::vector<int>         rxPacketsSizes;
         char*                    rxStreamBuf;
         int                      rxStreamBufSize,
                                  rxStreamBufDefSize;

         int            ethHeaderLen;
         int            bytesRequested;

      public:
         etherStreambuf();
         virtual ~etherStreambuf();

         bool openInterface(std::string iface, long int frameType);
         virtual void closeInterface();
         bool isInterfaceOpen();
         int  getInterfaceFD();
         void setRxTimeoutuSec(int usecs);
         void setRxTimeout(int ms);
         void setRxCollectTimeoutuSec(int usecs);
         void setRxCollectTimeout(int ms);

         virtual bool sendRawPacket(etherPacket& pkt, int pktLength);
         bool sendPacket(const std::string& smac,
                         const std::string& dmac,
                         unsigned long int frameType,
                         const std::string& data);

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
         void setTxMode(xmitMode txMode);
         void setRxMode(xmitMode rxMode);
         void setMTU(int mtu);
         void setMaxDataLen(int mdl);
         void setDataHeader(std::string dhdr);
         void setRxBufferSize(int size);

      protected:
         virtual void initValues();
         void setTxBuffer(int dhl, int mtu);
         bool sendDataPacket(etherPacket& pkt, int pktLength);
         virtual bool receiveRawPacket(etherPacket& pkt, int& pktLength);
         void fetchPackets(int timeout, int collectTimeout);
         virtual bool getNextDataPacket(etherPacket& pkt, int& pktLength);
         virtual int  peekNextDataPacketSize();
         void realignBuffer(int& bytesRequested);

         std::streamsize xsputn(const char_type *s,
                                std::streamsize n);
         int_type overflow(int_type c = traits_type::eof());
         int sync();

         std::streamsize xsgetn(char_type *s,
                                std::streamsize n);
         virtual std::streamsize showmanyc();
         int_type underflow();
         void flush();
   };
};

#endif
