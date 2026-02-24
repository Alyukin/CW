all: cw

cw: main.o additional.o tasks.o
	gcc main.o additional.o tasks.o -o cw

main.o: main.c tasks.h
	gcc -c main.c

additional.o: additional.c additional.h
	gcc -c additional.c

tasks.o: tasks.c tasks.h additional.h
	gcc -c tasks.c

clean:
	rm -rf *.o cw
