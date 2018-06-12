CC=g++
CFLAGS=-std=c++11
all: dt.exe

dt.exe: main.cc decisiontree.h util.h
	$(CC) ${CFLAGS} $< -o $@

old_dt.exe: main.cc decisiontree.h util.h
	$(CC) -std=c++0x $< -o $@

clean:
	rm -f ${all}
	rm -f *.o
