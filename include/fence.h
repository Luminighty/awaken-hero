#ifndef FENCE_H
#define FENCE_H

#include "collision.h"
#include <raylib.h>


typedef enum {
	FENCE_TL,
	FENCE_TR,
	FENCE_BL,
	FENCE_BR,
	FENCE_L,
	FENCE_R,
	FENCE_T,
	FENCE_B,
} FenceType;


typedef struct {
	EntityId id;
	FenceType type;
	Rectangle position;
	ColliderId collider;
} Fence;


Fence fence_create(FenceType type, int x, int y);
void fence_destroy(Fence* fence);
void fence_render(Fence* fence);


#endif // FENCE_H
