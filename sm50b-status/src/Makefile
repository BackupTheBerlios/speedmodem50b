default: sm50b

run:	sm50b sm50b.c
	./sm50b 169.254.1.8 -h

sm50b: sm50b.c
	gcc -o sm50b sm50b.c

clean:
	rm -f sm50b

edit: clean
	nano sm50b.c
