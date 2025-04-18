#include "map.h"
#include "config.h"
#include "textures.h"
#include "tile.h"
#include <raylib.h>
#include <stdio.h>


static void draw_empty_room(Room* room);

Map map_create() {
	Map map;
	draw_empty_room(&map.room);
	return map;
}

static inline void room_set_tile(Room* room, int x, int y, Tile tile) {
	room->tiles[y * ROOM_WIDTH + x] = tile;
}
static inline int room_xy_idx(int x, int y) {
	return y * ROOM_WIDTH + x;
}


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
}

void map_render(Map* map) {
	room_render(&map->room);
}


