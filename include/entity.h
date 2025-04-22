#ifndef ENTITY_H
#define ENTITY_H

#include <stddef.h>
#include <stdint.h>

// NOTE: ENTITY_TYPE, TypeDef, identifier, max_amount/room
#define MAP_ENTITY_TYPES \
	X(ENTITY_OWL, Owl, owl, 8) \
	X(ENTITY_POT, Pot, pot, 64) \
	X(ENTITY_CHEST, Chest, chest, 64) \
	X(ENTITY_FENCE, Fence, fence, 64) \
	X(ENTITY_DOOR, Door, door, 64) \


#define X(type, ...) type ,
typedef enum {
	ENTITY_UNKNOWN,
	ENTITY_HERO,
	ENTITY_NETWORK_HERO,
	MAP_ENTITY_TYPES
} EntityType;
#undef X


typedef struct {
	EntityType type;
	uint32_t id;
} EntityId;


extern char* ENTITY_TYPE_TO_STR[];

EntityId entity_create_id(EntityType type);
void* entity_lookup(EntityId);


#endif // ENTITY_H
