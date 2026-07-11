CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17
TARGET = chip8

INCLUDE_PATHS = -I"C:/msys64/ucrt64/include"
LIBRARY_PATHS = -L"C:/msys64/ucrt64/lib"

SRCS = src/main.cpp src/chip8.cpp src/tinyfiledialogs.c
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

ifeq ($(OS), Windows_NT)
    LIBS = -lmingw32 -lSDL2main -lSDL2 -lole32 -lcomdlg32
    RM = del /Q /F
    CLEAN_OBJS = $(subst /,\,$(OBJS))
    RUN_CMD = $(TARGET)
    EXT = .exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
        LIBS = -lSDL2main -lSDL2
        RM = rm -rf
        CLEAN_OBJS = $(OBJS)
        RUN_CMD = ./$(TARGET)
    endif
endif


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)$(EXT) $(OBJS) $(LIBRARY_PATHS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	$(RM) $(CLEAN_OBJS) $(TARGET)$(EXT)

run:
	$(RUN_CMD)