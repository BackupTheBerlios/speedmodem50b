default: sm50b

LIBPNGCONFIG := libpng-config

CFLAGS := $(shell $(LIBPNGCONFIG) --cflags)
LIBS := $(shell $(LIBPNGCONFIG) --libs)

run:	sm50b sm50b.c
	./sm50b 169.254.1.8 -h


sm50b: sm50b.c
	[ "$(OS)" == "Windows_NT" ] && make sm50b.exe || true
	[ "$(OS)" != "Windows_NT" ] && make sm50b.linux || true

sm50b.linux: sm50b.c
	gcc -o sm50b $(CFLAGS) $(LIBS) sm50b.c

sm50b.exe: sm50b.c
	gcc -o sm50b.exe $(CFLAGS) $(LIBS) sm50b.c

copybin: sm50b
	[ "$(OS)" == "Windows_NT" ] && make copybin.exe || true
	[ "$(OS)" != "Windows_NT" ] && make copybin.linux || true

copybin.linux: sm50b
	cp sm50b ../sm50b

copybin.exe: sm50b.exe
	cp sm50b.exe ../sm50b.exe

clean:
	rm -f sm50b sm50b.exe

edit: clean
	nano sm50b.c
