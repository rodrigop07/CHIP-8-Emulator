CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17
TARGET = chip8

SRCS = src/main.cpp src/chip8.cpp $(wildcard src/imgui/*.cpp)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

ifeq ($(OS), Windows_NT)
    INCLUDE_PATHS = -I"C:/msys64/ucrt64/include" -I"C:/msys64/ucrt64/include/SDL2"
    LIBRARY_PATHS = -L"C:/msys64/ucrt64/lib"
    LIBS = -lmingw32 -lSDL2main -lSDL2 -lole32 -lcomdlg32
    RM = del /Q /F
    CLEAN_OBJS = $(subst /,\,$(OBJS))
    RUN_CMD = $(TARGET)
    EXT = .exe
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Linux)
        INCLUDE_PATHS = $(shell pkg-config --cflags sdl2)
        LIBS = $(shell pkg-config --libs sdl2)
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