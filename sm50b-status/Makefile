default: sm50b.zip

sm50b.zip: copybin
	cp /bin/cygwin*.dll . && { cp ./cygwin*.dll ./bin/win32 || true; } || cp ./bin/win32/cygwin*.dll . || true
	cd .. && zip -9 sm50b-status/sm50b.zip sm50b-status sm50b-status/src sm50b-status/Makefile sm50b-status/src/COPYING sm50b-status/src/Makefile sm50b-status/src/sm50b.c sm50b-status/cygwin*.dll sm50b-status/sm50b sm50b-status/sm50b.exe sm50b-status/sm50b.static.exe sm50b-status/misc sm50b-status/misc/cgi sm50b-status/misc/cgi/sm50b.cgi || true
	rm -f cygwin*.dll || true
	make -C src clean

copybin:
	make -C src
	make -C src copybin
	test -f sm50b && { cp sm50b bin/linux || true; } || cp bin/linux/sm50b . || true
	test -f sm50b.static && { cp sm50b.static bin/linux || true; } || cp bin/linux/sm50b.static . || true
	test -f sm50b.exe && { cp sm50b.exe bin/win32 || true; } || cp bin/win32/sm50b.exe . || true
	test -f sm50b.static.exe && { cp sm50b.static.exe bin/win32 || true; } || cp bin/win32/sm50b.static.exe . || true

clean:
	rm -f sm50b sm50b.exe sm50b.static sm50b.static.exe sm50b.zip
