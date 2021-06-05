GCC = g++ -std=c++17 
MPICC = mpicxx -std=c++17 -g  
CXXFLAGS := -O0 -Wall -Wfatal-errors -pthread
MPICCFLAGS = -O0 -Wall -Wfatal-errors -pthread 
LIBRA = lib/libspdlog.a

LDFLAGS  := -L${HOME}/.local/lib -lgeos
MAKE_DIR = $(PWD)
OBJ_DIR  := $(MAKE_DIR)/bin
APP_DIR  := $(MAKE_DIR)
TEST_DIR := $(MAKE_DIR)/tests
TARGET   := agrider
INCLUDE  := -isystem ${HOME}/.local/include -Iinclude -I/usr/inclue -I/usr/local/include 
SRC      :=                      \
   $(wildcard src/*.cpp)         \
   $(wildcard tests/*.cpp)       \

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(GCC) $(CXXFLAGS) $(INCLUDE)  -o $(APP_DIR)/$(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: all build clean debug release

build:
	@mkdir -p $(OBJ_DIR)

debug: MPICCFLAGS += -DDEBUG -g
debug: all

release: MPICCFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/$(TARGET)
