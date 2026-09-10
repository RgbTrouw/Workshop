CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES = -Iinclude
SRC = src/main.cpp src/Game.cpp src/Terminal.cpp
TARGET = space_shuttle_game

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
