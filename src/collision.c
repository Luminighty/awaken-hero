#include "collision.h"
#include "config.h"
#include "game.h"
#include "map.h"
#include "tile.h"

#include <raylib.h>
#include <stdio.h>

#include <stdlib.h>
#include <assert.h>

CollisionServer server = {0};

ColliderId collider_create(Rectangle area) {
	Collider collider = {
		.area = area,
		.alive = true,
		.enabled = true,
	};

	bool found_slot = false;
	ColliderId id;
	for (size_t i = 0; i < server.colliders.count && !found_slot; i++)
		if (!server.colliders.items[i].alive) {
			found_slot = true;
			id = i;
			server.colliders.items[i] = collider;
		}
	if (!found_slot) {
		id = server.colliders.count;
		da_push(server.colliders, collider);
	}
	assert(server.colliders.items[id].alive);
	return id;
}

void collider_destroy(ColliderId id) { server.colliders.items[id].alive = false; }
void collider_set_enabled(ColliderId id, bool enabled) { server.colliders.items[id].enabled = enabled; }
void collider_set_debug(ColliderId id, bool debug) { server.colliders.items[id].debug = debug; }

static inline bool is_tile_solid(int x, int y) {
	Tile tile = map_get_tile(&game.map, x, y);
	return TILE_SOLID[tile];
}

static inline bool is_colliding_with_map(Rectangle area) {
	int from_x = area.x / TILE_SIZE;
	int from_y = area.y / TILE_SIZE;
	int to_x = (area.x + area.width) / TILE_SIZE;
	int to_y = (area.y + area.height) / TILE_SIZE;
	for (int y = from_y; y <= to_y; y++) {
	for (int x = from_x; x <= to_x; x++) {
		if (is_tile_solid(x, y))
			return true;
	}
	}
	return false;
}

void collision_render() {
	for (size_t i = 0; i < server.colliders.count; i++) {
		Collider* c = &server.colliders.items[i];
		if (c->alive && c->debug) {
			DrawRectangleLinesEx(c->area, 1.0f, GREEN);
		}
	}

}

#define AS_VEC2(area) (Vector2){.x = area.x, .y = area.y}

static inline bool rect_intersects(Rectangle left, Rectangle right) {
	return left.x < right.x + right.width &&
		left.x + left.width > right.x &&
		left.y < right.y + right.height &&
		left.y + left.height > right.y;
}

static inline bool collides_with(Collider* other, Rectangle area) {
	if (rect_intersects(area, other->area))
		return true;
	return false;
}

Vector2 collider_move(ColliderId id, Vector2 delta) {
	assert(server.colliders.items[id].alive);
	Collider* collider = &server.colliders.items[id];
	Rectangle new_area = {
		.x = collider->area.x + delta.x,
		.y = collider->area.y + delta.y,
		.width = collider->area.width,
		.height = collider->area.height
	};

	if (is_colliding_with_map(new_area))
		return AS_VEC2(collider->area);

	// TODO: Check with other colliders
	for (size_t other_id = 0; other_id < server.colliders.count; other_id++)
		if(id != other_id && collides_with(&server.colliders.items[other_id], new_area))
			return AS_VEC2(collider->area);

	server.colliders.items[id].area.x = new_area.x;
	server.colliders.items[id].area.y = new_area.y;
	return AS_VEC2(new_area);
}

