# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread

# Target executable
TARGET = ex3.out

# Source and header files
SRC_DIR = .
INC_DIR = .

SOURCES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INC_DIR)/*.h)
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Linking the object files to create the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

# Compiling each .c file into .o file
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove object files and the executable
clean:
	rm -f $(OBJECTS) $(TARGET)
