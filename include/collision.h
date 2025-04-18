#ifndef COLLISION_H
#define COLLISION_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "darray.h"

typedef size_t ColliderId;

typedef struct {
	Rectangle area;
	bool alive;
	bool enabled;
	bool debug;
} Collider;

DATypedef(Colliders, Collider);

typedef struct {
	Colliders colliders;
} CollisionServer;


void collision_render();

ColliderId collider_create(Rectangle area);
void collider_destroy(ColliderId id);

Vector2 collider_move(ColliderId id, Vector2 delta);
void collider_set_enabled(ColliderId id, bool enabled);
void collider_set_debug(ColliderId id, bool debug);

#endif // COLLISION_H
