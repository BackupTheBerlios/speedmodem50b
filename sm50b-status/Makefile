default: sm50b.zip

sm50b.zip: copybin
	cp /bin/cygwin*.dll . || true
	cd .. && zip -9 sm50b-status/sm50b.zip sm50b-status sm50b-status/src sm50b-status/Makefile sm50b-status/src/COPYING sm50b-status/src/Makefile sm50b-status/src/sm50b.c sm50b-status/cygwin*.dll sm50b-status/sm50b sm50b-status/sm50b.exe
#	rm -f cygwin*.dll || true
	make -C src clean

copybin:
	make -C src
	make -C src copybin

clean:
	rm -f sm50b sm50b.exe sm50b.zip
