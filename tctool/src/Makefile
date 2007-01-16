#**************************************************************************
#   Copyright (C) 2005 by Ralf Miunske                                    *
#   miunske at users.berlios.de                                           *
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
# $Id: Makefile,v 1.7 2007/01/16 16:15:54 miunske Exp $

DESTDIR  := /usr/local/bin/
MAIN     := tctool
MAINOBJ  := if.o interface.o etherStream.o etherStreambuf.o tcStream.o tcStreambuf.o tcRtsDump.o tcRtsDumpbuf.o
EXAMPLE  := example
EXOBJ    := if.o interface.o etherStream.o etherStreambuf.o tcStream.o tcStreambuf.o tcRtsDump.o tcRtsDumpbuf.o

#CROSS_COMPILE := mipsel-linux-uclibc-
CROSS_ARCH    := $(shell test -z "$(CROSS_COMPILE)" && echo -march=i386)
CC            := $(CROSS_COMPILE)gcc
LD            := $(CROSS_COMPILE)gcc
CXX           := $(CROSS_COMPILE)g++

CXXFLAGS   := -O2
LDFLAGS    := --strip-unneeded
LIBS       := 
LDSTATIC   := -L. -static-libgcc
STATICLIBS := stdc++ m
COUNTRY    := "C"
LANG       := "C"
LANGUAGE   := "C"
LC_ALL     := "C"

default: install

test: $(EXAMPLE)
	./$(EXAMPLE)

MIPS_PREFIX := mipsel-linux-uclibc-

$(MAIN).mipsel: clean
	export CROSS_COMPILE=$(MIPS_PREFIX); make $(MAIN)
	$(MIPS_PREFIX)-strip -o $(MAIN).mipsel $(MAIN)

all: $(MAIN) $(EXAMPLE) objclean docs

install: $(MAIN) installbinary

install.static: static installbinary

installbinary:
	install -s -p -o root -g root -m 4755 $(MAIN) $(DESTDIR)

uninstall: $(MAIN)
	rm -fv $(DESTDIR)/$(MAIN)

$(MAIN): $(MAIN).o $(MAINOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(LIBS) $^

static: staticlibs $(MAIN).static
	mv $(MAIN).static $(MAIN)

staticlibs:
	for LIB in $(STATICLIBS); do ln -sfv `$(CXX) -print-file-name=lib$${LIB}.a`; done

$(MAIN).static: $(MAIN).o $(MAINOBJ)
	$(CXX) $(LDSTATIC) $(LDFLAGS) -o $@ $(LIBS) $^

$(EXAMPLE): $(EXAMPLE).o $(EXOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(LIBS) $^

%.o: %.cpp %.h
	$(CXX) -c $(CROSS_ARCH) $(CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(CROSS_ARCH) $(CXXFLAGS) $< -o $@

%.o: %.c
	$(CXX) -c $(CROSS_ARCH) $(CXXFLAGS) $< -o $@

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
	rm -rfv $(MAIN) $(MAIN).static $(MAIN).mipsel $(EXAMPLE) autom4te.cache config.status config.log install-sh

objclean: libsclean
	rm -fv *~ *.o

libsclean:
	for LIB in $(STATICLIBS); do rm -fv "lib$${LIB}.a"; done

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
	tcRtsDump.h \
	tcRtsDump.cpp \
	tcRtsDumpbuf.h \
	tcRtsDumpbuf.cpp \
	tcStream.h \
	tcStream.cpp \
	tcStreambuf.h \
	tcStreambuf.cpp \
	>/dev/null 2>&1 &

backup: distclean
	cvs ci -m "automatic commitment"

