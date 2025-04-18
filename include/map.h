#ifndef MAP_H
#define MAP_H

#include "config.h"
#include "fence.h"
#include "tile.h"


typedef struct {
	Tile tiles[ROOM_SIZE];
	Fence fences[32];
	int fence_c;
} Room;

typedef struct {
	Room room;
} Map;

Map map_create();

void map_render(Map* map);

#endif // MAP_H
