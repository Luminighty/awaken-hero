#include "collision.h"
#include "config.h"
#include "entity.h"
#include "game.h"
#include "map.h"
#include "tile.h"

#include <float.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include <stdlib.h>
#include <assert.h>

CollisionServer server = {0};

ColliderId collider_create(EntityId parent, Rectangle area, CollisionLayer layer) {
	Collider collider = {
		.area = area,
		.alive = true,
		.enabled = true,
		.layer = layer,
	};
	collider.parent = parent;

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
	return other->enabled && rect_intersects(area, other->area);
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

static inline float solve_line_equation_for_x(float x, float y, float c, float target_y) {
	assert(fabs(x) > EPSILON);
	return -(y * target_y + c) / x;
}

static inline float solve_line_equation_for_y(float x, float y, float c, float target_x) {
	assert(fabs(y) > EPSILON);
	return -(x * target_x + c) / y;
}

static inline bool is_solution_in_range(float from, float length, float solution) {
	return from < solution && from + length > solution;
}

/// Returns true if a collider has been found that's closer than min_distance. Also updates result and min_distance based on this
static inline bool raycast_hit_collider(Collider* collider, float x, float y, float c, Vector2 from, Vector2* result, float* min_distance) {
	assert(collider);
	bool is_hit_closer = false;
	// We do not consider parallel lines
	if (fabs(x) > EPSILON) {
		float solution_top = solve_line_equation_for_x(x, y, c, collider->area.y);
		float solution_bottom = solve_line_equation_for_x(x, y, c, collider->area.y + collider->area.height);
		if (is_solution_in_range(collider->area.x, collider->area.width, solution_top)) {
			Vector2 point = (Vector2){ solution_top, collider->area.y };
			float distance = Vector2DistanceSqr(from, point);
			if (*min_distance > distance) {
				*result = point;
				*min_distance = distance;
				is_hit_closer = true;
			}
		}
		if (is_solution_in_range(collider->area.x, collider->area.width, solution_bottom)) {
			Vector2 point = (Vector2){ solution_bottom, collider->area.y + collider->area.height };
			float distance = Vector2DistanceSqr(from, point);
			if (*min_distance > distance) {
				*result = point;
				*min_distance = distance;
				is_hit_closer = true;
			}
		}
	}
	// We do not consider parallel lines
	if (fabs(y) > EPSILON) {
		float solution_left = solve_line_equation_for_y(x, y, c, collider->area.x);
		float solution_right = solve_line_equation_for_y(x, y, c, collider->area.x + collider->area.width);
		if (is_solution_in_range(collider->area.y, collider->area.height, solution_left)) {
			Vector2 point = (Vector2){ collider->area.x, solution_left };
			float distance = Vector2DistanceSqr(from, point);
			if (*min_distance > distance) {
				*result = point;
				*min_distance = distance;
				is_hit_closer = true;
			}
		}
		if (is_solution_in_range(collider->area.y, collider->area.height, solution_right)) {
			Vector2 point = (Vector2){ collider->area.x + collider->area.width, solution_right };
			float distance = Vector2DistanceSqr(from, point);
			if (*min_distance > distance) {
				*result = point;
				*min_distance = distance;
				is_hit_closer = true;
			}
		}
	}
	return is_hit_closer;
}

bool collider_raycast_hit(Raycast raycast, CollisionLayer hit_layer, RaycastHitResult* result) {
	assert(result);
	Vector2 from = raycast.origin;
	Vector2 to = Vector2Add(raycast.origin, raycast.point);
	float x = from.y - to.y;
	float y = to.x - from.x;
	float c = from.x * to.y - from.y * to.x;
	float min_distance = Vector2DistanceSqr(from, to);
	bool found;
	Vector2 min_distance_point;
	for (size_t id = 0; id < server.colliders.count; id++) {
		if (!(server.colliders.items[id].layer & hit_layer))
			continue;
		if (raycast_hit_collider(&server.colliders.items[id], x, y, c, from, &min_distance_point, &min_distance)) {
			found = true;
			result->parent = server.colliders.items[id].parent;
			result->collider = id;
		}
	}
	if (found)
		result->hit_point = min_distance_point;
	return found;
}

