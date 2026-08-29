CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g3
SOURCES = graphics.c utils.c sound.c eastereggs/cobra/cobra.c problems.c problems/problem_0.c problems/problem_1.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = main.exe

# Biblioteca BASS (áudio). Só é distribuído lib/bass.dll + lib/bass.def;
# o import lib (lib/libbass.a) usado pelo linker do MinGW é gerado
# automaticamente na primeira compilação, caso ainda não exista.
BASS_DIR = lib
BASS_DLL = $(BASS_DIR)/bass.dll
BASS_DEF = $(BASS_DIR)/bass.def
BASS_LIB = $(BASS_DIR)/libbass.a
LDFLAGS  = -L$(BASS_DIR) -lbass

all: $(TARGET)

$(TARGET): $(OBJECTS) bass.dll
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Objetos dependem do import lib para garantir que ele exista antes do link
$(OBJECTS): $(BASS_LIB)

# Gera o import lib (libbass.a) a partir do bass.dll + bass.def
# usando dlltool (parte do MinGW/binutils), caso ele não exista ainda.
$(BASS_LIB): $(BASS_DEF) $(BASS_DLL)
	dlltool -d $(BASS_DEF) -D bass.dll -l $(BASS_LIB)

# Copia o bass.dll para perto do executável, necessário em tempo de execução.
bass.dll: $(BASS_DLL)
	copy /Y "$(subst /,\,$(BASS_DLL))" bass.dll >nul

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJECTS) $(TARGET) bass.dll 2>nul || exit 0