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
// $Id: interface.cpp,v 1.2 2006/12/06 17:48:25 miunske Exp $

#include "interface.h"

int rp_openInterface(char const *ifname, UINT16_t type, char *hwaddr) {
   return openInterface(ifname, type, static_cast<unsigned char*>(static_cast<void*>(hwaddr)));
}

int rp_sendPacket(char const *ifname, int sock, const void *pkt, int size) {
   PPPoEConnection conn;
   conn.ifName=const_cast<char*>(ifname);
   return sendPacket(&conn, sock, static_cast<PPPoEPacket*>(const_cast<void*>(pkt)), size);
}

int rp_receivePacket(int sock, void *pkt, int *size) {
   return receivePacket(sock, static_cast<PPPoEPacket*>(pkt), size);
}

void fatalSys(char const *str) {
   return;
}

void rp_fatal(char const *str) {
   return;
}

void printErr(char const *str) {
   return;
}

void sysErr(char const *str) {
   return;
}
