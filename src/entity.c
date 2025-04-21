#include "entity.h"
#include "config.h"
#include "game.h"
#include <assert.h>
#include <stdint.h>


static uint32_t last_id;


EntityId entity_create_id(EntityType type) {
	return (EntityId){
		.type = type,
		.id = ++last_id
	};
}


inline static bool lookup_room(Room* room, EntityId id, void** result) {

#define X(TYPE, _Type, ident, _amount) \
case TYPE: \
	for(int i = 0; i < room->ident ## _c; i++) \
	if (room->ident ## s[i].id.id == id.id) { \
		*result = &room->ident ## s[i]; \
		return true; \
	} \
break; 

	switch (id.type) {
	default:
		break;
	MAP_ENTITY_TYPES
	}
	return false;

#undef X
}


void* entity_lookup(EntityId id) {
	void* result = NULL;
	switch (id.type) {
	case ENTITY_HERO:
		assert(game.hero.id.id == id.id);
		return &game.hero;
	case ENTITY_NETWORK_HERO:
		for (size_t i = 0; i < game.network_hero_count; i++)
		if (game.network_heroes[i].id.id == id.id)
			return &game.network_heroes[i];
		break;
	default:
		for(int y = 0; y < MAP_HEIGHT; y++) {
		for(int x = 0; x < MAP_WIDTH; x++) {
			if (lookup_room(&game.map.rooms[y][x], id, &result))
				return result;
		}}
	}
	return NULL;
}


#define X(TYPE, _Type, _ident, _amount) \
	[TYPE] = #TYPE,
char* ENTITY_TYPE_TO_STR[] = {
	[ENTITY_UNKNOWN] = "ENTITY_UNKNOWN",
	[ENTITY_HERO] = "ENTITY_HERO",
	[ENTITY_NETWORK_HERO] = "ENTITY_NETWORK_HERO",
	MAP_ENTITY_TYPES
};
#undef X
