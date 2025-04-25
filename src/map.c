#include "map.h"

#include "config.h"
#include "entity.h"
#include "fence.h"
#include "log.h"
#include "map_tiled.h"
#include "pot.h"
#include "door.h"
#include "textures.h"
#include "tile.h"

#include <raylib.h>
#include <assert.h>

#define LOG_HEADER "MAP"


static inline void room_set_tile(Room* room, int x, int y, Tile tile) { 
	room->tiles[y * ROOM_WIDTH + x] = tile;
}


static inline int room_xy_idx(int x, int y) { 
	return y * ROOM_WIDTH + x;
}


#define FOR_ROOM_TILES(x, y) \
	for (int y = 0; y < ROOM_HEIGHT; y++)\
	for (int x = 0; x < ROOM_WIDTH; x++)

#define FOR_ROOMS(x, y) \
	for (int y = 0; y < MAP_HEIGHT; y++)\
	for (int x = 0; x < MAP_WIDTH; x++)


static inline Rectangle rect_tile_dest(int x, int y) {
	return (Rectangle){
		.x = x * TILE_SIZE,
		.y = y * TILE_SIZE,
		.width = TILE_SIZE,
		.height = TILE_SIZE
	};
}


static void room_render(Room* room) {
	FOR_ROOM_TILES(x, y) {
		int idx = room_xy_idx(x, y);
		Tile tile = room->tiles[idx];
		DrawTexturePro(
			textures.tileset,
			TILE_RECT[tile],
			rect_tile_dest(x, y),
			(Vector2){0, 0},
			0,
			WHITE
		);
	}
}


// NOTE: I broke up map_render and map_objects_render since we're using the camera in one of them, and not the other
void map_render(Map* map, int room_x, int room_y) {
	room_render(&map->rooms[room_y][room_x]);
}


void map_objects_render(Map *map) {
	#define X(_1, _2, ident, _3) \
	for (int i = 0; i < map->ident ## _c; i++) \
		ident ## _render(&map->ident ## s[i]);
	MAP_ENTITY_TYPES
	#undef X
}


Map map_create() {
	return map_tiled_load("assets/maps/debug.json");
}


Room* map_get_room_from_tile(Map* map, int x, int y) {
	int room_x = x / ROOM_WIDTH;
	int room_y = y / ROOM_HEIGHT;
	return &map->rooms[room_y][room_x];
}


Tile map_get_tile(Map* map, int x, int y) {
	int room_x = x / ROOM_WIDTH;
	int tile_x = x % ROOM_WIDTH;
	int room_y = y / ROOM_HEIGHT;
	int tile_y = y % ROOM_HEIGHT;
	return map->rooms[room_y][room_x].tiles[room_xy_idx(tile_x, tile_y)];
}


void map_set_tile(Map* map, int x, int y, Tile tile, TileFlag tileflags) {
	int room_x = x / ROOM_WIDTH;
	int tile_x = x % ROOM_WIDTH;
	int room_y = y / ROOM_HEIGHT;
	int tile_y = y % ROOM_HEIGHT;
	room_set_tile(&map->rooms[room_y][room_x], tile_x, tile_y, tile);
}


Room* map_get_room_at(Map* map, int x, int y) {
	assert(x >= 0 && x < ROOM_WIDTH * MAP_WIDTH);
	assert(y >= 0 && y < ROOM_HEIGHT * MAP_HEIGHT);
	int room_x = x / ROOM_WIDTH;
	int room_y = y / ROOM_HEIGHT;
	return &map->rooms[room_y][room_x];
}


void convert_global_to_room_roomtile(Rectangle* position, int x, int y) {
	position->x = x % SCREEN_WIDTH;
	position->y = y % SCREEN_HEIGHT;
	position->width = TILE_SIZE;
	position->height = TILE_SIZE;
}


void map_print_profiler(Map* map) {
	LOG("Memory used %lu bytes.\n", sizeof(*map));
	LOG("\tRooms: %zu bytes.\n", sizeof(map->rooms));

	#define X(_T, type, ident, amount) \
	size_t ident##_c = map->ident##_c; \
	size_t ident##_space = amount; \
	size_t ident##_bytes = sizeof(map->ident##s); \
	LOG("\t" #type ": %zu/%zu using %zu bytes.\n", ident##_c, ident##_space, ident##_bytes);
	MAP_ENTITY_TYPES
	#undef X
}

