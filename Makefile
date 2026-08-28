CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g3
SOURCES = graphics.c utils.c eastereggs/cobra/cobra.c problems.c problems/problem_0.c problems/problem_1.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJECTS) $(TARGET) 2>nul || exit 0