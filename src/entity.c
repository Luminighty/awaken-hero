#include "entity.h"
#include "config.h"
#include "door.h"
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


void* entity_lookup(EntityId id) {
	#define X(TYPE, _Type, ident, _amount) \
	case TYPE: \
	for(int i = 0; i < game.map.ident ## _c; i++) \
		if (game.map.ident ## s[i].id.id == id.id) { \
			return &game.map.ident ## s[i]; \
		} \
	break; 

	switch (id.type) {
	case ENTITY_HERO:
		assert(game.hero.id.id == id.id);
		return &game.hero;
	case ENTITY_NETWORK_HERO:
		for (size_t i = 0; i < game.network_hero_count; i++)
		if (game.network_heroes[i].id.id == id.id)
			return &game.network_heroes[i];
		break;
	MAP_ENTITY_TYPES
	default:
		break;
	}
	#undef X
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
