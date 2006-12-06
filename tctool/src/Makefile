#**************************************************************************
#   Copyright (C) 2005 by Ralf Miunske                                    *
#   miunske at users.sourceforge.net                                      *
#                                                                         *
#   This program is free software; you can redistribute it and/or modify  *
#   it under the terms of the GNU General Public License as published by  *
#   the Free Software Foundation; either version 2 of the License, or     *
#   (at your option) any later version.                                   *
#                                                                         *
#   This program is distributed in the hope that it will be useful,       *
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#   GNU General Public License for more details. (see COPYING)            *
#                                                                         *
#   You should have received a copy of the GNU General Public License     *
#   along with this program; if not, write to the                         *
#   Free Software Foundation, Inc.,                                       *
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#                                                                         *
#   tctool for linux                                                      *
#   Tr*ndCh*p-utility                                                     *
#                                                                         *
#   LIC: GPL                                                              *
#                                                                         *
#**************************************************************************
# $Id: Makefile,v 1.3 2006/12/06 15:57:54 miunske Exp $

DESTDIR  := /usr/local/bin/
MAIN     := tctool
MAINOBJ  := if.o interface.o etherStream.o etherStreambuf.o tcStream.o tcStreambuf.o
EXAMPLE  := example
EXOBJ    := if.o interface.o etherStream.o etherStreambuf.o tcStream.o tcStreambuf.o

CXX      := g++
CXXFLAGS := -march=i386 -O2
LDFLAGS  := --strip-unneeded
LIBS     := 
LDSTATIC := -L. # -static
COUNTRY  := "C"
LANG     := "C"
LANGUAGE := "C"
LC_ALL   := "C"

default: install

test: $(EXAMPLE)
	./$(EXAMPLE)

all: $(MAIN) $(EXAMPLE) objclean docs

install: $(MAIN)
	install -s -p -o root -g root -m 4755 $(MAIN) $(DESTDIR)

uninstall: $(MAIN)
	rm -fv $(DESTDIR)/$(MAIN)

$(MAIN): $(MAIN).o $(MAINOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(LIBS) $^

$(MAIN).static: $(MAIN).o $(MAINOBJ)
	ln -sf $(shell $(CXX) -print-file-name=libstdc++.a )
	$(CXX) $(LDSTATIC) $(LDFLAGS) -o $@ $(LIBS) $^
	rm -f libstdc++.a

$(EXAMPLE): $(EXAMPLE).o $(EXOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(LIBS) $^

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.c
	$(CXX) -c $(CXXFLAGS) $< -o $@

config.h: configure
	touch install-sh
	./configure; rm -f install-sh

configure:
	autoconf

docs:
	mkdir -p ../doc
	doxygen Doxyfile

distclean: realclean
	true rm -fv config.h

realclean: clean
	rm -rfv configure

clean: docsclean objclean
	rm -rfv $(MAIN) $(MAIN).static $(EXAMPLE) autom4te.cache config.status config.log install-sh

objclean:
	rm -fv *~ *.o

docsclean:
	rm -rf ../doc/*

edit: objclean
	kate \
	Doxyfile \
	Makefile \
	tctool.cpp \
	example.cpp \
	etherStream.h \
	etherStream.cpp \
	etherStreambuf.h \
	etherStreambuf.cpp \
	tcStream.h \
	tcStream.cpp \
	tcStreambuf.h \
	tcStreambuf.cpp \
	>/dev/null 2>&1 &

backup: distclean
	cvs ci -m "automatic commitment"
