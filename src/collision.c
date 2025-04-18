#include "collision.h"
#include "config.h"
#include "game.h"
#include "tile.h"
#include <raylib.h>
#include <stdio.h>


bool is_tile_solid(int x, int y) {
	Tile tile = game.map.room.tiles[y * ROOM_WIDTH + x];
	return TILE_SOLID[tile];
}
