#include "game.h"
#include "client.h"
#include "collision.h"
#include "config.h"
#include "hero.h"
#include "map.h"
#include "network_hero.h"
#include "textures.h"
#include "tile.h"
#include <raylib.h>

Game game = {0};

void game_create() {
	InitWindow(WIN_WIDTH, WIN_HEIGHT, TITLE);
	textures_create();
	network_client_create();
	game.map = map_create();
	game.hero = hero_create();

	game.camera.zoom = 1.0f;
	game.camera.rotation = 0.0f;
	game.camera.offset.x = SCREEN_WIDTH / 2.f;
	game.camera.offset.y = SCREEN_HEIGHT / 2.f;

}

void game_destroy() {
	textures_destroy();
	network_client_destroy();
	CloseWindow();
}

void game_update() {
	tileset_update();
	hero_update(&game.hero);
	network_client_update();
	
	for (size_t i = 0; i < game.network_hero_count; i++)
		network_hero_update(&game.network_heroes[i]);

	game.camera.target.x = game.hero.room_x * TILE_SIZE * ROOM_WIDTH + TILE_SIZE * ROOM_WIDTH / 2.f;
	game.camera.target.y = game.hero.room_y * TILE_SIZE * ROOM_HEIGHT + TILE_SIZE * ROOM_HEIGHT / 2.f;
}

static const Rectangle RENDER_SOURCE = {.x = 0, .y = 0, .width = SCREEN_WIDTH, .height = -SCREEN_HEIGHT};
static const Rectangle RENDER_DEST = {.x = 0, .y = 0, .width = WIN_WIDTH, .height = WIN_HEIGHT};

void game_render() {
	BeginDrawing();
	BeginTextureMode(textures.render_target);

	ClearBackground(BLACK);
	map_render(&game.map, game.hero.room_x, game.hero.room_y);

	BeginMode2D(game.camera);
	
	for (size_t i = 0; i < game.network_hero_count; i++)
		network_hero_render(&game.network_heroes[i]);

	hero_render(&game.hero.husk);
	collision_render();
	EndMode2D();
	
	EndTextureMode();
	DrawTexturePro(textures.render_target.texture, RENDER_SOURCE, RENDER_DEST, (Vector2){0, }, 0, WHITE);
	EndDrawing();
}

bool game_is_running() {
	return !WindowShouldClose();
}

