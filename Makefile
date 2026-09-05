CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g3
SOURCES = graphics.c utils.c sound.c eastereggs/cobra/cobra.c problems.c problems/problem_0.c problems/problem_1.c problems/problem_2.c problems/problem_3.c problems/problem_4.c problems/problem_5.c problems/problem_6.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = main.exe

BASS_DIR = lib
BASS_DLL = $(BASS_DIR)/bass.dll
BASS_DEF = $(BASS_DIR)/bass.def
BASS_LIB = $(BASS_DIR)/libbass.a
LDFLAGS  = -L$(BASS_DIR) -lbass

all: $(TARGET)

$(TARGET): $(OBJECTS) bass.dll
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJECTS): $(BASS_LIB)

$(BASS_LIB): $(BASS_DEF) $(BASS_DLL)
	dlltool -d $(BASS_DEF) -D bass.dll -l $(BASS_LIB)

bass.dll: $(BASS_DLL)
	copy /Y "$(subst /,\,$(BASS_DLL))" bass.dll >nul

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(subst /,\,$(OBJECTS)) $(TARGET) bass.dll