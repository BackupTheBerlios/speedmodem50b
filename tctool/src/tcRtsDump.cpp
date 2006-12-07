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
// $Id: tcRtsDump.cpp,v 1.1 2006/12/07 03:25:28 miunske Exp $

#include "tcRtsDump.h"

namespace tc {
   tcRtsDump::tcRtsDump(const std::string& iface, const std::string& peer) {
      tcRtsDumpbuf* buf = rdbuf();
      if(buf!=NULL) delete buf;
      buf=new tcRtsDumpbuf(iface, peer);
      rdbuf(buf);
   }

   tcRtsDumpbuf* tcRtsDump::rdbuf() {
      return buf;
   }

   tcRtsDumpbuf* tcRtsDump::rdbuf(tcRtsDumpbuf* newbuf) {
      return buf=newbuf;
   }

   bool tcRtsDump::doSomething() {
      return buf->doSomething();
   }
}
