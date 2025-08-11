# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Executable name
TARGET = fs

# Source files
SRC = FS.c main.c stack.c queue.c

# Object files
OBJ = $(SRC:.c=.o)

# Build rule
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)

