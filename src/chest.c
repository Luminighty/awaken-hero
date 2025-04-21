#include "chest.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "map.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle chest_SPRITE = TILE(0, 5);

Chest chest_create(int x, int y) {
	Chest chest = {0};
	chest.id = entity_create_id(ENTITY_CHEST);
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	chest.collider = collider_create(chest.id, rect, COLLISION_LAYER_CHEST);
	collider_set_debug(chest.collider, true);
	convert_global_to_room_roomtile(&chest.position, x, y);
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

