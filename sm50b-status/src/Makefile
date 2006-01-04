default: sm50b

LIBPNGCONFIG := libpng-config
HAVELIBPNG := $(shell which $(LIBPNGCONFIG) 2>/dev/null >/dev/null && echo '-DHAVE_LIBPNG')
CFLAGS := $(shell $(LIBPNGCONFIG) --cflags)
LIBS := $(shell $(LIBPNGCONFIG) --libs)
STATIC := -static

run:	sm50b sm50b.c
	./sm50b 169.254.1.8 -h


sm50b: sm50b.c
	[ "$(OS)" == "Windows_NT" ] && make sm50b.exe || true
	[ "$(OS)" != "Windows_NT" ] && make sm50b.linux || true

sm50b.linux: sm50b.c
	gcc -o sm50b $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c

sm50b.static.linux: sm50b.c
	gcc -o sm50b.static $(STATIC) $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c

sm50b.exe: sm50b.c
	gcc -o sm50b.exe $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c

sm50b.static.exe: sm50b.c
	gcc -o sm50b.exe $(STATIC) $(CFLAGS) $(LIBS) $(HAVELIBPNG) sm50b.c

copybin: sm50b
	[ "$(OS)" == "Windows_NT" ] && make copybin.exe || true
	[ "$(OS)" != "Windows_NT" ] && make copybin.linux || true

copybin.linux: sm50b
	cp sm50b ../sm50b

copybin.exe: sm50b.exe sm50b.static.exe
	cp sm50b.exe ../sm50b.exe
	cp sm50b.static.exe ../sm50b.static.exe

clean:
	rm -f sm50b sm50b.exe sm50b.static sm50b.static.exe

edit: clean
	nano sm50b.c
