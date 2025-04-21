#ifndef MAP_H
#define MAP_H

#include "chest.h"
#include "config.h"
#include "fence.h"
#include "owl.h"
#include "pot.h"
#include "tile.h"


typedef struct {
	Tile tiles[ROOM_SIZE];
	Fence fences[32];
	int fence_c;
	Pot pots[16];
	int pot_c;
	Chest chests[8];
	int chest_c;
	Owl owls[4];
	int owl_c;
} Room;


// TODO: Consider using a sparse tilemap (size_t[y][x] -> Room)
typedef struct {
	Room rooms[MAP_HEIGHT][MAP_WIDTH];
	int spawn_x, spawn_y;
} Map;

Map map_create();
void map_render(Map* map, int room_x, int room_y);
Room* map_get_room_from_tile(Map* map, int x, int y);
Tile map_get_tile(Map* map, int x, int y);

void map_set_tile(Map* map, int x, int y, Tile tile);
Room* map_get_room_at(Map* map, int x, int y);


#endif // MAP_H
