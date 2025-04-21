#include "owl.h"

#include "config.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle OWL_SPRITE = TILE(4, 3);

Owl owl_create(int x, int y) {
	Owl owl = {0};
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	owl.collider = collider_create(rect);
	owl.position.x = x % SCREEN_WIDTH;
	owl.position.y = y % SCREEN_HEIGHT;
	owl.position.width = TILE_SIZE;
	owl.position.height = TILE_SIZE;
	return owl;
}


void owl_destroy(Owl* owl) {
	collider_destroy(owl->collider);
}


void owl_render(Owl* owl) {
	DrawTexturePro(
		textures.tileset,
		OWL_SPRITE,
		owl->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

