#include "switch_block.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "game.h"
#include "textures.h"
#include <raylib.h>

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle SWITCH_BLOCK_SPRITE_UP = TILE(6, 2);
static Rectangle SWITCH_BLOCK_SPRITE_DOWN = TILE(8, 2);
static Rectangle SWITCH_BLOCK_SPRITE_TRANS = TILE(7, 2);


SwitchBlock switch_block_create(int x, int y, bool enabled) {
	SwitchBlock switch_block = {0};
	switch_block.id = entity_create_id(ENTITY_SWITCH_BLOCK);
	Rectangle rect = { .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE };
	switch_block.collider = collider_create(switch_block.id, rect, COLLISION_LAYER_WALL);
	// collider_set_debug(switch_block.collider, true);
	switch_block.position = rect;
	switch_block.enabled = enabled;
	return switch_block;
}


void switch_block_destroy(SwitchBlock* switch_block) {
	collider_destroy(switch_block->collider);
}


void switch_block_render(SwitchBlock* switch_block) {
	Rectangle rect = 
		game.map.is_switch_toggling ? SWITCH_BLOCK_SPRITE_TRANS :
		switch_block->enabled ? SWITCH_BLOCK_SPRITE_UP : 
			SWITCH_BLOCK_SPRITE_DOWN;
	DrawTexturePro(
		textures.tileset,
		rect,
		switch_block->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

void switch_block_toggle_start(SwitchBlock* switch_block) {
	collider_set_enabled(switch_block->collider, true);
}


void switch_block_toggle_finish(SwitchBlock* switch_block) {
	switch_block->enabled = !switch_block->enabled;
	collider_set_enabled(switch_block->collider, switch_block->enabled);
}
