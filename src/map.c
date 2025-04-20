#include "map.h"
#include "config.h"
#include "fence.h"
#include "textures.h"
#include "tile.h"
#include <raylib.h>
#include <assert.h>


static void draw_empty_room(Room* room);


static inline void room_set_tile(Room* room, int x, int y, Tile tile) { room->tiles[y * ROOM_WIDTH + x] = tile; }
static inline int room_xy_idx(int x, int y) { return y * ROOM_WIDTH + x; }


#define FOR_ROOM_TILES(x, y) \
	for (int y = 0; y < ROOM_HEIGHT; y++)\
	for (int x = 0; x < ROOM_WIDTH; x++)

static inline void draw_line_h(Room* room, int y, int from_x, int to_x, Tile tile) {
	for (int x = from_x; x < to_x; x++)
		room->tiles[room_xy_idx(x, y)] = tile;

}
static inline void draw_line_v(Room* room, int x, int from_y, int to_y, Tile tile) {
	for (int y = from_y; y < to_y; y++)
		room->tiles[room_xy_idx(x, y)] = tile;
}

static inline void fill(Room* room, int from_x, int to_x, int from_y, int to_y, Tile tile) {
	for (int y = from_y; y < to_y; y++)\
	for (int x = from_x; x < to_x; x++)
		room->tiles[room_xy_idx(x, y)] = tile;
}

static void draw_empty_room(Room* room) {
	fill(room, 0, ROOM_WIDTH - 1, 0, ROOM_HEIGHT - 1, TILE_FLOOR);
	draw_line_h(room, 0, 0, ROOM_WIDTH, TILE_WALL_T);
	draw_line_h(room, ROOM_HEIGHT - 1, 0, ROOM_WIDTH - 1, TILE_WALL_B);
	draw_line_v(room, 0, 0, ROOM_HEIGHT, TILE_WALL_L);
	draw_line_v(room, ROOM_WIDTH - 1, 0, ROOM_HEIGHT, TILE_WALL_R);

	room_set_tile(room, 0, 0, TILE_WALL_TL);
	room_set_tile(room, ROOM_WIDTH - 1, 0, TILE_WALL_TR);
	room_set_tile(room, 0, ROOM_HEIGHT - 1, TILE_WALL_BL);
	room_set_tile(room, ROOM_WIDTH - 1, ROOM_HEIGHT - 1, TILE_WALL_BR);
}


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

	for (int i = 0; i < room->fence_c; i++)
		fence_render(&room->fences[i]);
}

void map_render(Map* map, int room_x, int room_y) {
	room_render(&map->rooms[room_y][room_x]);
}

/*
  TODO: Remove this once it's not needed (reading maps from files)
 
static void room_add_fence(Room* room, FenceType type, int x, int y) {
	assert(sizeof(room->fences) / sizeof(room->fences[0]) > (size_t)room->fence_c);
	room->fences[room->fence_c++] = fence_create(type, x * TILE_SIZE, y * TILE_SIZE);
}
*/

enum {
	DOOR_UP = 1 << 0,
	DOOR_DOWN = 1 << 1,
	DOOR_LEFT = 1 << 2,
	DOOR_RIGHT = 1 << 3,
	ROOM_STATUES = 1 << 4,
	ROOM_BLOCKS = 1 << 5,
	ROOM_TORCHES = 1 << 6,
	ROOM_CAULDRON = 1 << 7,
	ROOM_ALT_FLOOR = 1 << 8,
};

static void room_create(Room* room, int config) {
	draw_empty_room(room);
	if (config & DOOR_UP)
		room_set_tile(room, ROOM_WIDTH / 2, 0, TILE_FLOOR);
	if (config & DOOR_DOWN)
		room_set_tile(room, ROOM_WIDTH / 2, ROOM_HEIGHT - 1, TILE_FLOOR);
	if (config & DOOR_LEFT)
		room_set_tile(room, 0, ROOM_HEIGHT / 2, TILE_FLOOR);
	if (config & DOOR_RIGHT)
		room_set_tile(room, ROOM_WIDTH - 1, ROOM_HEIGHT / 2, TILE_FLOOR);
	if (config & ROOM_STATUES) {
		room_set_tile(room, 2, 2, TILE_STATUE);
		room_set_tile(room, ROOM_WIDTH - 3, 2, TILE_STATUE);
	}
	if (config & ROOM_BLOCKS) {
		room_set_tile(room, 2, ROOM_HEIGHT - 3, TILE_BLOCK);
		room_set_tile(room, ROOM_WIDTH - 3, ROOM_HEIGHT - 3, TILE_BLOCK);
	}
	if (config & ROOM_ALT_FLOOR) {
		room_set_tile(room, 5, 4, TILE_FLOOR_ALT);
		room_set_tile(room, 5, 5, TILE_FLOOR_ALT);
	}
	if (config & ROOM_CAULDRON) {
		room_set_tile(room, ROOM_WIDTH / 2, ROOM_HEIGHT / 2, TILE_CAULDRON);
	}
	if (config & ROOM_TORCHES) {
		room_set_tile(room, 2, 0, TILE_TORCH);
		room_set_tile(room, 3, 0, TILE_TORCH);
		room_set_tile(room, ROOM_WIDTH - 3, 0, TILE_TORCH);
		room_set_tile(room, ROOM_WIDTH - 4, 0, TILE_TORCH);
	}
}


Map map_create() {
	Map map = {0};
	for (int y = 0; y < MAP_HEIGHT; y++) {
	for (int x = 0; x < MAP_WIDTH; x++) {
		int idx = y * MAP_WIDTH + x;
		int config = idx << 4;
		config |= x != 0 ? DOOR_LEFT : 0;
		config |= x != MAP_WIDTH - 1 ? DOOR_RIGHT : 0;
		config |= y != 0 ? DOOR_UP : 0;
		config |= y != MAP_HEIGHT - 1 ? DOOR_DOWN : 0;
		room_create(&map.rooms[y][x], config);
	}}

	return map;
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
