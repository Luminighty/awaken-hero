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


// TODO: Consider using a sparse tilemap (size_t[y][x] -> Room)
typedef struct {
	Room rooms[MAP_HEIGHT][MAP_WIDTH];
} Map;

Map map_create();
void map_render(Map* map, int room_x, int room_y);
Room* map_get_room_from_tile(Map* map, int x, int y);
Tile map_get_tile(Map* map, int x, int y);


#endif // MAP_H
