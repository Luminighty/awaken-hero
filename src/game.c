#include "game.h"
#include "config.h"
#include "hero.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>


Game game;

void game_create() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
	textures_create();
	game.map = map_create();
	game.hero = hero_create();
}

void game_destroy() {
	textures_destroy();
	CloseWindow();
}

void game_update() {
	hero_update(&game.hero);
}

void game_render() {
	BeginDrawing();
	ClearBackground(BLACK);
	map_render(&game.map);
	hero_render(&game.hero);
	EndDrawing();
}

bool game_is_running() {
	return !WindowShouldClose();
}

