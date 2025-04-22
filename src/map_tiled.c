#include "map_tiled.h"

#include "chest.h"
#include "config.h"
#include "door.h"
#include "json.h"
#include "log.h"
#include "map.h"
#include "tile.h"
#include "utils.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#define LOG_HEADER "MAP_TILED"

#define TILED_GUID unsigned int

typedef enum {
	OBJECT_NONE = 0,
	OBJECT_SPAWN,
	
	// NOTE: Make sure the order of these match up to FENCE_X
	OBJECT_FENCE_TL,
	OBJECT_FENCE_TR,
	OBJECT_FENCE_BL,
	OBJECT_FENCE_BR,
	OBJECT_FENCE_L,
	OBJECT_FENCE_R,
	OBJECT_FENCE_T,
	OBJECT_FENCE_B,

	OBJECT_POT,
	OBJECT_CHEST,
	OBJECT_OWL,
	OBJECT_KEYDOOR,

	// NOTE: Make sure the order of these match up to DOOR_TYPE_X
	OBJECT_DOOR_MONSTER,
	OBJECT_DOOR_KEY,
	OBJECT_DOOR_BOSS,
	OBJECT_DOOR_ONEWAY,
	OBJECT_DOOR_ONEWAY_EXIT,
	OBJECT_DOOR_KEYBLOCK,
} TileObject;


static Tile TILED_TO_TILE[];
static TileObject TILED_TO_TILEOBJECT[];


static const TILED_GUID TILED_FLIP_H =  0x80000000;
static const TILED_GUID TILED_FLIP_V =  0x40000000;
static const TILED_GUID TILED_FLIP_D =  0x20000000;
static const TILED_GUID TILED_ROTATED = 0x10000000;
static const TILED_GUID TILED_GUID_PART = ~(TILED_FLIP_H | TILED_FLIP_V | TILED_FLIP_D | TILED_ROTATED);

static void tile_object_create(Map* map, int x, int y, TileObject object);
static void read_tilelayer(Map* map, JsonValue* layer, int width);
static void read_objectgroup(Map* map, JsonValue* layer, int width);

Map map_tiled_load(const char *filename) {
	JsonValue* map_data = json_loadf(filename);

	// int height = json_as_number(json_object_get(map_data, "height"));
	int width = json_as_number(json_object_get(map_data, "width"));
	
	Map map = {0};

	JsonValue* layers = json_object_get(map_data, "layers");
	int layer_count = json_array_length(layers);
	for (int i = 0; i < layer_count; i++) {
		JsonValue* layer = json_array_get(layers, i);
		JsonString* type = json_as_string(json_object_get(layer, "type"));

		if (strcmp(type->string, "tilelayer") == 0)
			read_tilelayer(&map, layer, width);
		if (strcmp(type->string, "objectgroup") == 0)
			read_objectgroup(&map, layer, width);
	}
	fflush(stdout);

	json_destroy(map_data);
	return map;
}


static JsonValue* find_property_value(JsonValue* properties, char* property) {
	if (!properties)
		return NULL;
	int length = json_array_length(properties);
	for (int i = 0; i < length; i++) {
		JsonValue* property_object = json_array_get(properties, i);
		JsonString* name = json_as_string(json_object_get(property_object, "name"));
		if (strcmp(name->string, property) == 0)
			return json_object_get(property_object, "value");
	}
	return NULL;
}

static void object_owl(Map* map, int x, int y, JsonValue* properties) {
	assert_array_push(map->owls, map->owl_c);
	Owl owl = owl_create(x, y);
	JsonValue* message = find_property_value(properties, "message");
	if (message)
		strcpy(owl.message, json_as_string(message)->string);
	map->owls[map->owl_c++] = owl;
}

static void object_chest(Map* map, int x, int y, JsonValue* properties) {
	static const char* loot_table[] =  {
		[CHEST_LOOT_NOTHING] = "NOTHING",
		[CHEST_LOOT_RANDOM] = "RANDOM",
		[CHEST_LOOT_KEY] = "KEY",
		[CHEST_LOOT_BOSSKEY] = "KEY_BOSS",
	};
	static const int loot_table_length = sizeof(loot_table) / sizeof(loot_table[0]);
	assert_array_push(map->chests, map->chest_c);
	ChestLoot loot = CHEST_LOOT_NOTHING;
	JsonValue* prop_loot = find_property_value(properties, "loot");

	if (prop_loot) {
		JsonString* loot_str = json_as_string(prop_loot);
		for (int i = 0; i < loot_table_length; i++)
			if (strcmp(loot_table[i], loot_str->string) == 0)
				loot = i;
	}

	map->chests[map->chest_c++] = chest_create(x, y, loot);
}

static void read_objectgroup(Map* map, JsonValue* layer, int width) {
	JsonValue* data = json_object_get(layer, "objects");
	int data_length = json_array_length(data);
	for (int i = 0; i < data_length; i++) {
		JsonValue* object = json_array_get(data, i);
		JsonString* template = json_as_string(json_object_get(object, "template"));
		// NOTE: For some reason we have to move the y coord back, since top left is (0, 16) in Tiled for objects
		long double x = json_as_number(json_object_get(object, "x"));
		long double y = json_as_number(json_object_get(object, "y")) - TILE_SIZE;
		JsonValue* properties = json_object_get(object, "properties");

		Room* room = map_get_room_at(map, x / TILE_SIZE, y / TILE_SIZE);
		if (strcmp(template->string, "owl.tx") == 0) {
			object_owl(map, x, y, properties);
		} else if (strcmp(template->string, "chest.tx") == 0) {
			object_chest(map, x, y, properties);
		} else {
			LOG("Unknown template '%s'", template->string);
		}
		// TODO: Add other templates
	}
}


static void read_tilelayer(Map* map, JsonValue* layer, int width) {
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
			map_set_tile(map, x, y, tile);
		TileObject object = TILED_TO_TILEOBJECT[id];
		if (object != OBJECT_NONE)
			tile_object_create(map, x, y, object);
	}
	
}

static void tile_object_create(Map* map, int x, int y, TileObject object) {
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
	case OBJECT_FENCE_B: {
		assert_array_push(map->fences, map->fence_c);
		FenceType type = object - OBJECT_FENCE_TL;
		map->fences[map->fence_c++] = fence_create(type, x * TILE_SIZE, y * TILE_SIZE);
		break;
	}
	case OBJECT_POT:
		assert_array_push(map->pots, map->pot_c);
		map->pots[map->pot_c++] = pot_create(x * TILE_SIZE, y * TILE_SIZE);
		break;
	case OBJECT_CHEST:
		assert_array_push(map->chests, map->chest_c);
		map->chests[map->chest_c++] = chest_create(x * TILE_SIZE, y * TILE_SIZE, CHEST_LOOT_NOTHING);
		break;
	case OBJECT_OWL:
		assert_array_push(map->owls, map->owl_c);
		map->owls[map->owl_c++] = owl_create(x * TILE_SIZE, y * TILE_SIZE);
		break;
	case OBJECT_DOOR_MONSTER:
	case OBJECT_DOOR_KEY:
	case OBJECT_DOOR_BOSS:
	case OBJECT_DOOR_ONEWAY:
	case OBJECT_DOOR_ONEWAY_EXIT:
	case OBJECT_DOOR_KEYBLOCK: {
		assert_array_push(map->owls, map->owl_c);
		DoorType type = object - OBJECT_DOOR_MONSTER;
		map->doors[map->door_c++] = door_create(x * TILE_SIZE, y * TILE_SIZE, type);
		break;
	}
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

	
	[40] = OBJECT_DOOR_MONSTER,
	[41] = OBJECT_DOOR_KEY,
	[42] = OBJECT_DOOR_BOSS,
	[53] = OBJECT_DOOR_ONEWAY,
	[43] = OBJECT_DOOR_ONEWAY_EXIT,
	[52] = OBJECT_DOOR_KEYBLOCK,
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


#define assert_door(door)\
	static_assert(\
	OBJECT_DOOR_ ## door - OBJECT_DOOR_MONSTER == DOOR_TYPE_ ##  door, \
	"Invalid door mapping! OBJECT_" #door " -> " #door ". ");
assert_door(MONSTER)
assert_door(KEY)
assert_door(BOSS)
assert_door(ONEWAY)
assert_door(ONEWAY_EXIT)
assert_door(KEYBLOCK)
#undef assert_fence
