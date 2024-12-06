all: virusDetector

virusDetector: AntiVirus.o
	gcc -m32 -g -Wall -o virusDetector AntiVirus.o

AntiVirus.o: AntiVirus.c
	gcc -m32 -g -Wall -c -o AntiVirus.o AntiVirus.c

.PHONY: clean

clean:
	rm -f *.o virusDetector