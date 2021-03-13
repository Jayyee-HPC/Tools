GCC = g++ -std=c++17 
LIB = -lgeos -L${HOME}/.local/lib
CFLAGS = -O0 -Wall -Wfatal-errors -I/usr/local/include -I${HOME}/.local/include

OBJ := wkt_to_wkb wkb_to_wkt prog

all: wkt_to_wkb wkb_to_wkt
	
wkt_to_wkb: wkt_to_wkb.cpp
	$(GCC) $(CFLAGS) $(LIB) -o wkt_to_wkb wkt_to_wkb.cpp 

wkb_to_wkt: wkb_to_wkt.cpp
	$(GCC) $(CFLAGS) $(LIB) -o wkb_to_wkt wkb_to_wkt.cpp
	
clean:
	-@rm -f $(OBJ)
