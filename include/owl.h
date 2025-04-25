#ifndef OWL_H
#define OWL_H

#include "collision.h"
#include "entity.h"
#include <raylib.h>


typedef struct {
	EntityId id;
	Rectangle position;
	ColliderId collider;
	char message[32];
} Owl;


Owl owl_create(int x, int y);
void owl_destroy(Owl* owl);
void owl_render(Owl* owl);
void owl_on_interact(Owl* owl);


#endif // OWL_H
