#ifndef COLLISION_H
#define COLLISION_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "darray.h"
#include "entity.h"

typedef size_t ColliderId;


typedef enum {
	COLLISION_LAYER_PLAYER = 1 << 0,
	COLLISION_LAYER_SWORD = 1 << 1,
	COLLISION_LAYER_ENEMY = 1 << 2,
	COLLISION_LAYER_CHEST = 1 << 3,
	COLLISION_LAYER_POT = 1 << 4,
	COLLISION_LAYER_OWL = 1 << 5,
	COLLISION_LAYER_FENCE = 1 << 6,
	COLLISION_LAYER_DOOR = 1 << 7,
} CollisionLayer;


typedef struct {
	EntityId parent;
	CollisionLayer layer;
	Rectangle area;
	bool alive;
	bool enabled;
	bool debug;
} Collider;


DATypedef(Colliders, Collider);

typedef struct {
	Colliders colliders;
} CollisionServer;


typedef struct {
	EntityId parent;
	ColliderId collider;
	Vector2 hit_point;
} RaycastHitResult;


typedef struct {
	Vector2 origin;
	Vector2 point;
} Raycast;


void collision_render();

ColliderId collider_create(EntityId parent, Rectangle area, CollisionLayer layer);
void collider_destroy(ColliderId id);

Vector2 collider_move(ColliderId id, Vector2 delta);
void collider_set_enabled(ColliderId id, bool enabled);
void collider_set_debug(ColliderId id, bool debug);
void collider_set_parent(ColliderId id, EntityType parent_type, void* parent);

bool collider_raycast_hit(Raycast raycast, CollisionLayer hit_layer, RaycastHitResult* result);


#endif // COLLISION_H
