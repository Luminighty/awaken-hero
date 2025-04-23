#ifndef MAP_H
#define MAP_H

#include "config.h"
#include "entity.h"
#include "chest.h"
#include "door.h"
#include "fence.h"
#include "owl.h"
#include "pot.h"
#include "tile.h"
#include <raylib.h>


#define X(_, Type, ident, amount) \
	Type ident ## s [amount]; \
	int ident ## _c;

typedef struct {
	Tile tiles[ROOM_SIZE];
} Room;



// TODO: Consider using a sparse tilemap (size_t[y][x] -> Room)
// NOTE: Maybe we could count the amount of ENTITY required per map, and alloc for that
//	Although this is fine for now too.
typedef struct {
	Room rooms[MAP_HEIGHT][MAP_WIDTH];
	MAP_ENTITY_TYPES
	int spawn_x, spawn_y;
} Map;
#undef X

Map map_create();
void map_render(Map* map, int room_x, int room_y);
void map_objects_render(Map* map);
Room* map_get_room_from_tile(Map* map, int x, int y);
Tile map_get_tile(Map* map, int x, int y);

void map_set_tile(Map* map, int x, int y, Tile tile, TileFlipFlag flipflags);
Room* map_get_room_at(Map* map, int x, int y);

// NOTE: This function converts a global position to a tile relative to a room
//	This is used for most of the tile bound map objects
void convert_global_to_room_roomtile(Rectangle* position, int x, int y);
void map_print_profiler(Map* map);


#endif // MAP_H
