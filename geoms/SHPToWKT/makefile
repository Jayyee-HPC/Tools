GCC = g++
CC = /opt/openmpi/bin/mpicxx -g
LIBB = -lgeos -I/opt/geos/include
CFLAGS = -O0 -Wall -Wfatal-errors -c -std=c++0x -I/usr/local/include
LIBRA = lib/libshp.a lib/librtree.a
DEBUG = -g

LFLAGS = -Wfatal-errors

#-DNDEBUG add this to disable assert macro

#grid: grid.o
all:shp2wkt.o SHPReader.o 
	$(CC) $(LFLAGS) -o prog shp2wkt.o SHPReader.o $(LIBB) $(LIBRA)	
	
shp2wkt.o: shp2wkt.cpp
	$(CC) $(CFLAGS) shp2wkt.cpp

adaptiveGrider1.o: adaptiveGrider1.cpp
	$(CC) $(CFLAGS) adaptiveGrider1.cpp
	
SHPReader.o: SHPReader.cpp
	$(CC) $(CFLAGS) SHPReader.cpp $(LIBB)	
	
clean:
	rm SHPReader.o shp2wkt.o prog 
 
