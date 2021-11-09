all: reader writer

reader: macrosLD.h
		gcc -Wall noncanonical.c -o r

writer: macrosLD.h
		gcc -Wall writenoncanonical.c -o wr

clean:
		rm r wr