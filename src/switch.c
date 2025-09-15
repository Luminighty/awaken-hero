#include "switch.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "textures.h"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle SWITCH_SPRITE = TILE(6, 3);


Switch switch_create(int x, int y) {
	Switch _switch = {0};
	_switch.id = entity_create_id(ENTITY_SWITCH);
	Rectangle rect = { .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE };
	_switch.collider = collider_create(_switch.id, rect, COLLISION_LAYER_SWITCH);
	// collider_set_debug(_switch.collider, true);
	_switch.position = rect;
	return _switch;
}


void switch_destroy(Switch* _switch) {
	collider_destroy(_switch->collider);
}


void switch_render(Switch* _switch) {
	DrawTexturePro(
		textures.tileset,
		SWITCH_SPRITE,
		_switch->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

