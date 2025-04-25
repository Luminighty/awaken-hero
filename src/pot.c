#include "pot.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle POT_SPRITE = TILE(1, 6);


Pot pot_create(int x, int y) {
	Pot pot = {0};
	pot.id = entity_create_id(ENTITY_POT);
	Rectangle rect = { .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE };
	pot.collider = collider_create(pot.id, rect, COLLISION_LAYER_POT);
	// collider_set_debug(pot.collider, true);
	pot.position = rect;
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

