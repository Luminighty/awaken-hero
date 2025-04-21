#include "chest.h"

#include "config.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle chest_SPRITE = TILE(0, 5);

Chest chest_create(int x, int y) {
	Chest chest = {0};
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	chest.collider = collider_create(rect);
	chest.position.x = x % SCREEN_WIDTH;
	chest.position.y = y % SCREEN_HEIGHT;
	chest.position.width = TILE_SIZE;
	chest.position.height = TILE_SIZE;
	return chest;
}


void chest_destroy(Chest* chest) {
	collider_destroy(chest->collider);
}


void chest_render(Chest* chest) {
	DrawTexturePro(
		textures.tileset,
		chest_SPRITE,
		chest->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}
