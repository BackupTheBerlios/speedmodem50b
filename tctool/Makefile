default: copybin

tctool.zip: copybin
	rm -f sm50b.zip
	cd .. && zip -9 sm50b-status/sm50b.zip sm50b-status sm50b-status/src sm50b-status/Makefile sm50b-status/src/COPYING sm50b-status/src/Makefile sm50b-status/src/sm50b.c sm50b-status/cygwin*.dll sm50b-status/sm50b sm50b-status/sm50b.exe sm50b-status/misc sm50b-status/misc/cgi sm50b-status/misc/cgi/sm50b.cgi || true
	rm -f cygwin*.dll || true
	make -C src clean

copybin:
	make -C src tctool
	test -f src/tctool && cp src/tctool bin || true
	test -f src/tctool && cp src/tctool . || true

clean:
	rm -f tctool bin/tctool
	make -C src clean
