CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17
TARGET = chip8

INCLUDE_PATHS = -I"C:/msys64/ucrt64/include"
LIBRARY_PATHS = -L"C:/msys64/ucrt64/lib"

SRCS = main.cpp chip8.cpp
OBJS = $(SRCS:.cpp=.o)

ifeq ($(OS), Windows_NT)
    LIBS = -lmingw32 -lSDL2main -lSDL2
    RM = del /Q /F
    CLEAN_OBJS = $(subst /,\,$(OBJS))
    RUN_CMD = $(TARGET)
    EXT = .exe
endif


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)$(EXT) $(OBJS) $(LIBRARY_PATHS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	$(RM) $(CLEAN_OBJS) $(TARGET)$(EXT)

run:
	$(RUN_CMD)