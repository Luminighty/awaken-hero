#include "map_tiled.h"

#include "json.h"
#include "map.h"
#include "tile.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#define TILED_GUID unsigned int

typedef enum {
	OBJECT_NONE = 0,
	OBJECT_SPAWN,
	OBJECT_POT,
	
	// NOTE: Make sure the order of these match up to FENCE_X
	OBJECT_FENCE_TL,
	OBJECT_FENCE_TR,
	OBJECT_FENCE_BL,
	OBJECT_FENCE_BR,
	OBJECT_FENCE_L,
	OBJECT_FENCE_R,
	OBJECT_FENCE_T,
	OBJECT_FENCE_B,

	OBJECT_CHEST,
	OBJECT_OWL,
} TileObject;


static Tile TILED_TO_TILE[];
static TileObject TILED_TO_TILEOBJECT[];


static const TILED_GUID TILED_FLIP_H =  0x80000000;
static const TILED_GUID TILED_FLIP_V =  0x40000000;
static const TILED_GUID TILED_FLIP_D =  0x20000000;
static const TILED_GUID TILED_ROTATED = 0x10000000;
static const TILED_GUID TILED_GUID_PART = ~(TILED_FLIP_H | TILED_FLIP_V | TILED_FLIP_D | TILED_ROTATED);

static void tile_object_create(Map* map, int x, int y, TileObject object);

Map map_tiled_load(const char *filename) {
	JsonValue* map_data = json_loadf(filename);

	// int height = json_as_number(json_object_get(map_data, "height"));
	int width = json_as_number(json_object_get(map_data, "width"));
	
	Map map = {0};

	JsonValue* layers = json_object_get(map_data, "layers");
	int layer_count = json_array_length(layers);
	for (int i = 0; i < layer_count; i++) {
		JsonValue* layer = json_array_get(layers, i);

		JsonValue* data = json_object_get(layer, "data");
		int data_length = json_array_length(data);

		for (int j = 0; j < data_length; j++) {
			int x = j % width;
			int y = j / width;
			TILED_GUID id = json_as_number(json_array_get(data, j));
			id = id & TILED_GUID_PART;
			if (id == 0)
				continue;
			id--;
			Tile tile = TILED_TO_TILE[id];
			if (tile != TILE_NONE)
				map_set_tile(&map, x, y, tile);
			TileObject object = TILED_TO_TILEOBJECT[id];
			if (object != OBJECT_NONE)
				tile_object_create(&map, x, y, object);
		}
	}
	fflush(stdout);

	json_destroy(map_data);
	return map;
}

static void tile_object_create(Map* map, int x, int y, TileObject object) {
	#define assert_array_push(array, count) assert(sizeof(array) / sizeof((array)[0]) > (size_t)(count))
	Room* room = map_get_room_at(map, x, y);
	switch (object) {
	case OBJECT_SPAWN:
		map->spawn_x = x;
		map->spawn_y = y;
		break;
	case OBJECT_FENCE_TL:
	case OBJECT_FENCE_TR:
	case OBJECT_FENCE_BL:
	case OBJECT_FENCE_BR:
	case OBJECT_FENCE_L:
	case OBJECT_FENCE_R:
	case OBJECT_FENCE_T:
	case OBJECT_FENCE_B:
		assert_array_push(room->fences, room->fence_c);
		FenceType type = object - OBJECT_FENCE_TL;
		room->fences[room->fence_c++] = fence_create(type, x * TILE_SIZE, y * TILE_SIZE);
		break;
	case OBJECT_POT:
		assert_array_push(room->pots, room->pot_c);
		room->pots[room->pot_c++] = pot_create(x * TILE_SIZE, y * TILE_SIZE);
		break;
	case OBJECT_CHEST:
		assert_array_push(room->chests, room->chest_c);
		room->chests[room->chest_c++] = chest_create(x * TILE_SIZE, y * TILE_SIZE);
		break;
	case OBJECT_OWL:
		assert_array_push(room->owls, room->owl_c);
		room->owls[room->owl_c++] = owl_create(x * TILE_SIZE, y * TILE_SIZE);
		break;
	default:
		printf("TileObject %d not implemented yet.\n", object);
		assert(0);
		break;
	}

}


static Tile TILED_TO_TILE[] = {
	[0] = TILE_WALL_TL,
	[1] = TILE_WALL_T,
	[2] = TILE_WALL_TR,
	[10] = TILE_WALL_L,
	[11] = TILE_FLOOR,
	[12] = TILE_WALL_R,
	[20] = TILE_WALL_BL,
	[21] = TILE_WALL_B,
	[22] = TILE_WALL_BR,
	[3] = TILE_WALL_CORNER_TL,
	[4] = TILE_WALL_CORNER_TR,
	[13] = TILE_WALL_CORNER_BL,
	[14] = TILE_WALL_CORNER_BR,

	[24] = TILE_CEIL,
	[31] = TILE_FLOOR_ALT,
	[32] = TILE_FLOOR_STAIR,
	[63] = TILE_BLOCK,
	[62] = TILE_STATUE,
	[05] = TILE_HOLE,
	[15] = TILE_HOLE_B,
	[25] = TILE_HOLE_T,
	[35] = TILE_HOLE_TB,

	[6] = TILE_CAULDRON,
	[7] = TILE_CAULDRON,
	[8] = TILE_CAULDRON,
	[9] = TILE_CAULDRON,
	[16] = TILE_TORCH,
	[17] = TILE_TORCH,
	[18] = TILE_TORCH,
	[19] = TILE_TORCH,
};

static TileObject TILED_TO_TILEOBJECT[] = {
	[33] = OBJECT_SPAWN,
	[44] = OBJECT_FENCE_TL,
	[45] = OBJECT_FENCE_TR,
	[54] = OBJECT_FENCE_BL,
	[55] = OBJECT_FENCE_BR,
	[46] = OBJECT_FENCE_L,
	[47] = OBJECT_FENCE_R,
	[56] = OBJECT_FENCE_T,
	[57] = OBJECT_FENCE_B,
	[61] = OBJECT_POT,
	[50] = OBJECT_CHEST,
	[34] = OBJECT_OWL,
};

#define str(s) #s

#define assert_fence(fence)\
	static_assert(\
	OBJECT_ ## fence - OBJECT_FENCE_TL == fence, \
	"Invalid fence mapping! OBJECT_" #fence " -> " #fence ". ")
assert_fence(FENCE_TL);
assert_fence(FENCE_TR);
assert_fence(FENCE_BL);
assert_fence(FENCE_BR);
assert_fence(FENCE_L);
assert_fence(FENCE_R);
assert_fence(FENCE_T);
assert_fence(FENCE_B);
#undef assert_fence
