#include "switch.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "game.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>
#include <stdio.h>

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle SWITCH_SPRITE = TILE(6, 3);
static Rectangle SWITCH_SPRITE_ENABLED = TILE(7, 3);
static const float INVULNERABILITY_PADDING_DELAY = 0.1f;


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
		(_switch->invulnerability > 0.0f) ? SWITCH_SPRITE_ENABLED : SWITCH_SPRITE,
		_switch->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

void switch_update(Switch* _switch) {
	if (_switch->invulnerability > 0.f) {
		_switch->invulnerability -= GetFrameTime();
		if (_switch->invulnerability <= 0.f)
			map_switch_toggle_finish(&game.map);
	}
}

void switch_on_hit(Switch* _switch, float invulnerability) {
	if (_switch->invulnerability > 0)
		return;
	_switch->invulnerability = invulnerability + INVULNERABILITY_PADDING_DELAY;
	map_switch_toggle_start(&game.map);
}
