CC = gcc
CFLAGS = -I./include -Wall
LDFLAGS = -L./bin
LDFLAGS_SHARED = -L./bin
SRC = src/main.c src/game_loader.c
OUT = main
OUT_GAME = libgamelib

OUT_DIR = out

GAME_SRC = $(wildcard src/game/*.c)

ifeq ($(OS),Windows_NT)
	OUT := $(OUT).exe
	OUT_GAME := $(OUT_GAME).dll
	LDFLAGS += -lraylib -lopengl32 -lgdi32 -lwinmm
	LDFLAGS_SHARED += -shared -lraylib -lopengl32 -lgdi32 -lwinmm
else
	OUT_GAME := $(OUT_GAME).dylib
	LDFLAGS += -lraylib.550 -Wl,-rpath,@loader_path/../bin -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo -framework OpenGL
	LDFLAGS_SHARED += -dynamiclib -lraylib.550 -Wl,-rpath,@loader_path/../bin -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo -framework OpenGL
endif

.PHONY: game

all: game main

make_dirs:
	mkdir -p out

$(OUT_DIR)/$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT_DIR)/$(OUT) $(CFLAGS) $(LDFLAGS)

ifeq ($(OS),Windows_NT)
$(OUT_DIR)/$(OUT_GAME): $(GAME_SRC) $(OUT_DIR)/raylib.dll
else
$(OUT_DIR)/$(OUT_GAME): $(GAME_SRC)
endif
	$(CC) $(GAME_SRC) -o $(OUT_DIR)/$(OUT_GAME) $(CFLAGS) $(LDFLAGS_SHARED)

main: make_dirs
	make $(OUT_DIR)/$(OUT)

game: make_dirs
	make $(OUT_DIR)/$(OUT_GAME)

ifeq ($(OS),Windows_NT)

$(OUT_DIR)/raylib.dll:
	cmd /c copy .\bin\raylib.dll .\out\

endif

clean:
	rm -rf $(OUT_DIR)
