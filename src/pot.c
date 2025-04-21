#include "pot.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "map.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle POT_SPRITE = TILE(1, 6);

Pot pot_create(int x, int y) {
	Pot pot = {0};
	pot.id = entity_create_id(ENTITY_POT);
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	pot.collider = collider_create(pot.id, rect, COLLISION_LAYER_POT);
	convert_global_to_room_roomtile(&pot.position, x, y);
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

