CC=gcc

all: libmodbuspayloader.a modbusframe

libmodbuspayloader.a: modbusPayloader.o
	ar rcs libmodbuspayloader.a modbusPayloader.o

modbusPayloader.o: modbusPayloader.c modbusPayloader.h
	${CC} -c -o modbusPayloader.o -I ./ modbusPayloader.c

modbusframe: modbusPayloader.o modbusFrame.c
	${CC} -o modbusframe -I ./ modbusPayloader.o modbusFrame.c

install: all
	cp libmodbuspayloader.a /usr/local/lib
	cp modbusPayloader.h /usr/local/include
	cp modbusframe /usr/local/bin

uninstall:
	rm /usr/local/lib/libmodbuspayloader.a
	rm /usr/local/include/modbusPayloader.h
	rm /usr/local/bin/modbusframe

clean:
	rm *.a *.o modbusframe
