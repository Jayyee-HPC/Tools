GCC = g++ -std=c++17 
LDFLAGS := -lgeos -L${HOME}/.local/lib
CFLAGS := -O0 -Wall -Wfatal-errors 
INCLUDE := -isystem ${HOME}/.local/include -I/usr/local/include -Iinclude

OBJ := wkt_to_mbr prog

all: wkt_to_mbr
	
wkt_to_mbr: wkt_to_mbr.cpp
	$(GCC) $(CFLAGS) $(INCLUDE) -o wkt_to_mbr wkt_to_mbr.cpp $(LDFLAGS)
	
clean:
	-@rm -f $(OBJ)
