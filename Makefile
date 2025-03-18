CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -lm -lGL -lSDL2_mixer

SRC = src/main.c src/game.c src/player.c src/projectile.c src/enemy.c src/input.c src/graphics.c src/audio.c
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
EXEC = build/starfall  # Nome mais descritivo

ASSETS_DIR = assets
BUILD_DIR = build

.PHONY: all create_dir copy_assets clean

all: create_dir copy_assets $(EXEC)

create_dir:
	@mkdir -p $(BUILD_DIR)

copy_assets:
	@mkdir -p $(BUILD_DIR)/assets
	@cp -r $(ASSETS_DIR)/* $(BUILD_DIR)/assets/
	@echo "Assets copiados para $(BUILD_DIR)/assets/"

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)/*.o $(EXEC) $(BUILD_DIR)/assets
	@echo "Build limpo!"