FILELIST := tctool tctool/doc tctool/doc/.keepme tctool/src tctool/src/Doxyfile tctool/src/Makefile tctool/src/config.h tctool/src/config.h.linux26 tctool/src/etherStream.cpp tctool/src/etherStream.h tctool/src/etherStreambuf.cpp tctool/src/etherStreambuf.h tctool/src/example.cpp tctool/src/if.c tctool/src/if.h tctool/src/interface.cpp tctool/src/interface.h tctool/src/pppoe.h tctool/src/tcRtsDump.h tctool/src/tcRtsDump.cpp tctool/src/tcRtsDumpbuf.h tctool/src/tcRtsDumpbuf.cpp tctool/src/tcStream.cpp tctool/src/tcStream.h tctool/src/tcStreambuf.cpp tctool/src/tcStreambuf.h tctool/src/tctool.cpp tctool/Makefile tctool/COPYING tctool/README tctool/misc tctool/misc/cgi tctool/misc/cgi/tc.cgi tctool/bin tctool/bin/tctool tctool/bin/tctool.static tctool/bin/.keepme

default: tctool.zip

tctool.zip: copybin
	rm -f sm50b.zip
	make -C src distclean
	cd .. && zip -9 tctool/tctool.zip $(FILELIST) || true
	rm -f cygwin*.dll || true
	make -C src clean

copybin:
	make -C src tctool staticlibs tctool.static
	test -f src/tctool && cp src/tctool bin || true
	test -f src/tctool.static && cp src/tctool.static bin || true
#	test -f src/tctool && cp src/tctool . || true
#	test -f src/tctool.static && cp src/tctool.static . || true

install:
	make -C src install

clean:
	rm -f tctool.zip tctool bin/tctool
	make -C src clean
