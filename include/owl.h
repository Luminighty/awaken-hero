#ifndef OWL_H
#define OWL_H

#include "collision.h"
#include <raylib.h>


typedef struct {
	Rectangle position;
	ColliderId collider;
} Owl;

Owl owl_create(int x, int y);
void owl_destroy(Owl* owl);
void owl_render(Owl* owl);

#endif // OWL_H
