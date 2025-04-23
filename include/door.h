#ifndef DOOR_H
#define DOOR_H

#include "collision.h"
#include "hero.h"
#include "tile.h"
#include <raylib.h>


typedef enum {
	DOOR_TYPE_MONSTER,
	DOOR_TYPE_KEY,
	DOOR_TYPE_BOSS,
	DOOR_TYPE_ONEWAY,
	DOOR_TYPE_ONEWAY_EXIT,
	DOOR_TYPE_KEYBLOCK,
} DoorType;

typedef struct {
	EntityId id;
	TileFlipFlag flipflags;
	Rectangle position;
	ColliderId collider;
	DoorType type;
	bool is_open;
} Door;

Door door_create(int x, int y, DoorType type, TileFlipFlag flipflags);
void door_destroy(Door* door);
void door_render(Door* door);
void door_on_interact(Door* door, Hero* hero);
void door_set_open(Door* door, bool open);

#endif // DOOR_H
