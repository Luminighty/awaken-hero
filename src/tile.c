#include "tile.h"
#include "config.h"
#include <raylib.h>


#define TILE(_x, _y) ((Rectangle){ .x = _x * TILE_SIZE, .y = _y * TILE_SIZE, .width = TILE_SIZE, .height = TILE_SIZE })

Rectangle TILE_RECT[] = {
	[TILE_WALL_TL] = TILE(0, 0),
	[TILE_WALL_T] = TILE(1, 0),
	[TILE_WALL_TR] = TILE(2, 0),
	[TILE_WALL_L] = TILE(0, 1),
	[TILE_FLOOR] = TILE(1, 1),
	[TILE_WALL_R] = TILE(2, 1),
	[TILE_WALL_BL] = TILE(0, 2),
	[TILE_WALL_B] = TILE(1, 2),
	[TILE_WALL_BR] = TILE(2, 2),
	[TILE_CEIL] = TILE(4, 2),
	[TILE_WALL_CORNER_TL] = TILE(3, 0),
	[TILE_WALL_CORNER_TR] = TILE(4, 0),
	[TILE_WALL_CORNER_BL] = TILE(3, 1),
	[TILE_WALL_CORNER_BR] = TILE(4, 1),
	[TILE_FLOOR_ALT] = TILE(1, 3),
	[TILE_FLOOR_STAIR] = TILE(2, 3),
	[TILE_BLOCK] = TILE(3, 6),
	[TILE_STATUE] = TILE(2, 6),
	[TILE_HOLE] = TILE(5, 0),
	[TILE_HOLE_B] = TILE(5, 1),
	[TILE_HOLE_T] = TILE(5, 2),
	[TILE_HOLE_TB] = TILE(5, 3),
};

bool TILE_SOLID[] = {
	[TILE_WALL_TL] = true,
	[TILE_WALL_T] = true,
	[TILE_WALL_TR] = true,
	[TILE_WALL_L] = true,
	[TILE_WALL_R] = true,
	[TILE_WALL_BL] = true,
	[TILE_WALL_B] = true,
	[TILE_WALL_BR] = true,
	[TILE_CEIL] = true,
	[TILE_WALL_CORNER_TL] = true,
	[TILE_WALL_CORNER_TR] = true,
	[TILE_WALL_CORNER_BL] = true,
	[TILE_WALL_CORNER_BR] = true,
	[TILE_BLOCK] = true,
	[TILE_STATUE] = true,
};
