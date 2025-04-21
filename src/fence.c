#include "fence.h"
#include "collision.h"
#include "config.h"
#include "entity.h"
#include "map.h"
#include "textures.h"
#include <raylib.h>

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
static Rectangle FENCE_RECT[] = {
	[FENCE_TL] = RECT(0, 0, 8, 8),
	[FENCE_TR] = RECT(8, 0, 8, 8),
	[FENCE_BL] = RECT(0, 8, 8, 8),
	[FENCE_BR] = RECT(8, 8, 8, 8),
	[FENCE_L] = RECT(0, 0, 8, 16),
	[FENCE_R] = RECT(8, 0, 8, 16),
	[FENCE_T] = RECT(0, 0, 16, 8),
	[FENCE_B] = RECT(0, 8, 16, 8),
};

#define TILE(x, y) (RECT(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE))
static Rectangle FENCE_SPRITE[] = {
	[FENCE_TL] = TILE(4, 4),
	[FENCE_TR] = TILE(5, 4),
	[FENCE_BL] = TILE(4, 5),
	[FENCE_BR] = TILE(5, 5),
	[FENCE_L] = TILE(6, 4),
	[FENCE_R] = TILE(7, 4),
	[FENCE_T] = TILE(6, 5),
	[FENCE_B] = TILE(7, 5),
};

Fence fence_create(FenceType type, int x, int y) {
	Fence fence = {0};
	fence.id = entity_create_id(ENTITY_FENCE);
	fence.type = type;
	Rectangle rect = FENCE_RECT[type];
	rect.x += x;
	rect.y += y;
	fence.collider = collider_create(fence.id, rect, COLLISION_LAYER_FENCE);
	convert_global_to_room_roomtile(&fence.position, x, y);
	return fence;
}

void fence_destroy(Fence *fence) {
	collider_destroy(fence->collider);
}

void fence_render(Fence *fence) {
	DrawTexturePro(
		textures.tileset,
		FENCE_SPRITE[fence->type],
		fence->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}

