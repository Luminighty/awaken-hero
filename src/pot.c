#include "pot.h"

#include "config.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle POT_SPRITE = TILE(1, 6);

Pot pot_create(int x, int y) {
	Pot pot = {0};
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	pot.collider = collider_create(rect);
	pot.position.x = x % SCREEN_WIDTH;
	pot.position.y = y % SCREEN_HEIGHT;
	pot.position.width = TILE_SIZE;
	pot.position.height = TILE_SIZE;
	return pot;
}


void pot_destroy(Pot* pot) {
	collider_destroy(pot->collider);
}


void pot_render(Pot* pot) {
	DrawTexturePro(
		textures.tileset,
		POT_SPRITE,
		pot->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

