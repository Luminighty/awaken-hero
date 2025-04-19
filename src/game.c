#include "game.h"
#include "client.h"
#include "collision.h"
#include "config.h"
#include "hero.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>

Game game;

void game_create() {
	InitWindow(WIN_WIDTH, WIN_HEIGHT, TITLE);
	textures_create();
	network_client_create();
	game.map = map_create();
	game.hero = hero_create();

}

void game_destroy() {
	textures_destroy();
	network_client_destroy();
	CloseWindow();
}

void game_update() {
	hero_update(&game.hero);
	network_client_update();
}

static const Rectangle RENDER_SOURCE = {.x = 0, .y = 0, .width = SCREEN_WIDTH, .height = -SCREEN_HEIGHT};
static const Rectangle RENDER_DEST = {.x = 0, .y = 0, .width = WIN_WIDTH, .height = WIN_HEIGHT};

void game_render() {
	BeginDrawing();
	BeginTextureMode(textures.render_target);
	ClearBackground(BLACK);
	map_render(&game.map);
	for (size_t i = 0; i < game.hero_husk_count; i++)
		hero_render(&game.hero_husks[i]);
	hero_render(&game.hero.husk);
	collision_render();
	EndTextureMode();
	DrawTexturePro(textures.render_target.texture, RENDER_SOURCE, RENDER_DEST, (Vector2){0, }, 0, WHITE);
	EndDrawing();
}

bool game_is_running() {
	return !WindowShouldClose();
}

