# Compiler
CC = g++
CFLAGS = -I./inc -Wall -Wextra

BUILD = build

# Source file
SRC_FILES = ./src/tetris.cpp
SRC_FILES += main.cpp

# Linker flags
LINKER_FLAGS = `sdl2-config --cflags --libs sdl2` -lSDL2_ttf

# Target name
TARGET = tetris.o

# Building target inside /build
$(BUILD)/$(TARGET): $(SRC_FILES)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ $(LINKER_FLAGS) -o $@

clean:
	rm -f ./$(BUILD)/$(TARGET)
	rm -rf $(BUILD)