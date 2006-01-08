default: sm50b

LIBPNGCONFIG := libpng-config
LIBPNGCONFIG := $(shell which $(LIBPNGCONFIG) 2>/dev/null)
HAVELIBPNG := $(shell test "$(LIBPNGCONFIG)" && echo '-DHAVE_LIBPNG')
CFLAGS := $(shell test "$(LIBPNGCONFIG)" && $(LIBPNGCONFIG) --cflags 2>/dev/null)
LIBS := $(shell test "$(LIBPNGCONFIG)" && $(LIBPNGCONFIG) --ldflags 2>/dev/null)
STATIC := -static

run:	sm50b sm50b.c
	./sm50b 169.254.1.8 -h


sm50b: sm50b.c
	[ "$(OS)" = "Windows_NT" ] && make sm50b.exe || true
	[ "$(OS)" != "Windows_NT" ] && make sm50b.linux || true

sm50b.linux: sm50b.c
	gcc -o sm50b $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c || make sm50b.linux.nolibpng

sm50b.linux.nolibpng: sm50b.c libpngerror
	gcc -o sm50b sm50b.c

sm50b.static.linux: sm50b.c
	gcc -o sm50b.static $(STATIC) $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c || make sm50b.static.linux.nolibpng

sm50b.static.linux.nolibpng: sm50b.c libpngerror
	gcc -o sm50b.static $(STATIC) sm50b.c

sm50b.exe: sm50b.c
	gcc -o sm50b.exe $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c || make sm50b.exe.nolibpng

sm50b.exe.nolibpng: sm50b.c libpngerror
	gcc -o sm50b.exe sm50b.c

sm50b.static.exe: sm50b.c
	gcc -o sm50b.static.exe $(STATIC) $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c || make sm50b.static.exe.nolibpng

sm50b.static.exe.nolibpng: sm50b.c libpngerror
	gcc -o sm50b.static.exe $(STATIC) sm50b.c

libpngerror:
	echo 'WARNING: Something went wront using libpng! Continuing without libpng...'

copybin: sm50b
	[ "$(OS)" = "Windows_NT" ] && make copybin.exe || true
	[ "$(OS)" != "Windows_NT" ] && make copybin.linux || true

copybin.linux: sm50b
	cp sm50b ../sm50b

copybin.exe: sm50b.exe sm50b.static.exe
	cp sm50b.exe ../sm50b.exe
	cp sm50b.static.exe ../sm50b.static.exe

clean:
	rm -f sm50b.o sm50b sm50b.exe sm50b.static.o sm50b.static sm50b.static.exe

edit: clean
	nano sm50b.c
