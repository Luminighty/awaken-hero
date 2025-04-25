#include "door.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "textures.h"
#include "tile.h"
#include <assert.h>
#include <raylib.h>

#define LOG_HEADER "DOOR"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle DOOR_SPRITE[] = {
	[DOOR_TYPE_MONSTER] = TILE(0, 4),
	[DOOR_TYPE_KEY] = TILE(1, 4),
	[DOOR_TYPE_BOSS] = TILE(2, 4),
	[DOOR_TYPE_ONEWAY] = TILE(3, 5),
	[DOOR_TYPE_ONEWAY_EXIT] = TILE(3, 4),
	[DOOR_TYPE_KEYBLOCK] = TILE(2, 5),
};


Door door_create(int x, int y, DoorType type, TileFlag flipflags) {
	Door door = {0};
	door.type = type;
	door.flipflags = flipflags;
	door.id = entity_create_id(ENTITY_DOOR);
	Rectangle rect = { .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE };
	door.collider = collider_create(door.id, rect, COLLISION_LAYER_DOOR);
	// collider_set_debug(door.collider, true);
	door.position = rect;
	return door;
}


void door_destroy(Door* door) {
	collider_destroy(door->collider);
}


void door_render(Door* door) {
	if (door->is_open)
		return;
	static const Vector2 origin = {TILE_SIZE / 2.f, TILE_SIZE / 2.f};
	Rectangle sprite = DOOR_SPRITE[door->type];

	float rotation = 0.f;
	TileFlag flags = door->flipflags;
	if (flags & TILE_ROTATED)
		rotation = 270.f;
	if (flags & TILE_FLIP_H)
		sprite.width *= -1;
	if (flags & TILE_FLIP_V)
		sprite.height *= -1;
	
	DrawTexturePro(
		textures.tileset,
		DOOR_SPRITE[door->type],
		(Rectangle){
			.x = door->position.x + origin.x,
			.y = door->position.y + origin.y,
			.width = door->position.width,
			.height = door->position.height,
		},
		origin,
		rotation,
		WHITE
	);
}


void door_on_interact(Door* door, Hero* hero) {
	if (door->is_open)
		return;
	switch (door->type) {
	case DOOR_TYPE_KEYBLOCK:
	case DOOR_TYPE_KEY:
		if (hero->inventory.keys == 0)
			return;
		hero->inventory.keys--;
		door_set_open(door, true);
		break;
	case DOOR_TYPE_BOSS:
		if (!hero->inventory.boss_key)
			return;
		door_set_open(door, true);
		break;
	case DOOR_TYPE_ONEWAY:
		// TODO: Move to other side of the door
	case DOOR_TYPE_MONSTER:
		// TODO: Implement Monsters
	case DOOR_TYPE_ONEWAY_EXIT:
	default:
		break;
	}
}


void door_set_open(Door* door, bool open) {
	assert(door);
	door->is_open = open;
	collider_set_enabled(door->collider, !open);
}

