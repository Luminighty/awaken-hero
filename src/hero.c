#include "hero.h"

#include "audio.h"
#include "chest.h"
#include "config.h"
#include "door.h"
#include "entity.h"
#include "game.h"
#include "log.h"
#include "map.h"
#include "message.h"
#include "switch.h"
#include "textures.h"
#include "collision.h"
#include "client.h"
#include "tile.h"
#include "utils.h"
#include "owl.h"

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>


#define LOG_HEADER "HERO"

#define VEC(_x, _y) ((Vector2){ .x = _x * TILE_SIZE, .y = _y * TILE_SIZE})
#define RECT(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE + 1), .width = TILE_SIZE, .height = TILE_SIZE })
#define RECTF(_x, _y) ((Rectangle){ .x = _x * (TILE_SIZE + 1), .y = _y * (TILE_SIZE + 1), .width = -TILE_SIZE, .height = TILE_SIZE })
#define NONE_SPRITE ((Rectangle){ .x = 7 * (TILE_SIZE + 1), .y = 0 * (TILE_SIZE + 1), .width = -TILE_SIZE, .height = -TILE_SIZE })

const float HERO_SPEED = 80;
static const Vector2 origin = { 3, 6 };
static const Vector2 size = { 10, 10 };
static const float WALK_DELAY = 0.4f;
static const float FLIP_DELAY = 0.2f;
static const float SWING_DELAY = 0.23f;
static const float FALL_DELAY = 0.6f;
static const Vector2 ROOM_OFFSET = (Vector2){.x = 5.f, .y = 0.f };
static const Rectangle FALL_SPRITES[] = {RECT(6, 1), RECT(7, 1), RECT(8, 1), RECT(9, 1)};

static const Rectangle SWORD_SPRITES[][3] = {
	[DIR_DOWN] = {RECT(0, 1), RECT(0, 2), RECT(1, 2)},
	[DIR_LEFT] = {RECT(1, 0), RECT(0, 0), RECT(0, 1)},
	[DIR_RIGHT] = {RECT(1, 0), RECT(2, 0), RECT(2, 1)},
	[DIR_UP] = {RECT(2, 1), RECT(2, 0), RECT(1, 0)},
};
static const Vector2 SWORD_OFFSETS[][3] = {
	[DIR_DOWN] = {VEC(-0.8, 0), VEC(-0.7, 0.8), VEC(0, 1)},
	[DIR_LEFT] = {VEC(0, -1), VEC(-0.8, -0.7), VEC(-1, 0)},
	[DIR_RIGHT] = {VEC(0.4, -1), VEC(0.8, -0.7), VEC(1, 0)},
	[DIR_UP] = {VEC(0.9, -0.4), VEC(0.75, -0.75), VEC(0, -1)},
};
static const Rectangle SWORD_AREA[][3] = {
	[DIR_DOWN] = {
		((Rectangle){-18.0, 1.0, 20.0, 10.0}),
		((Rectangle){-16.0, 4.0, 21.0, 21.0}),
		((Rectangle){3.0, 12.0, 10.0, 20.0}),
	},
	[DIR_LEFT] = {
		((Rectangle){-4.0, -22.0, 10.0, 20.0}),
		((Rectangle){-18.0,-18.0, 21.0, 21.0}),
		((Rectangle){-24.0, 0.0, 20.0, 10.0}),
	},
	[DIR_RIGHT] = {
		((Rectangle){2.0, -22.0, 10.0, 22.0}),
		((Rectangle){6.0,-18.0, 21.0, 21.0}),
		((Rectangle){14.0, 0.0, 20.0, 10.0}),
	},
	[DIR_UP] = {
		((Rectangle){10.0, -6.0, 20.0, 10.0}),
		((Rectangle){6.0, -20.0, 21.0, 21.0}),
		((Rectangle){-4.0, -26.0, 10.0, 20.0}),
	},
};

#define RAYCAST(_x, _y, _dx, _dy) (Raycast){ \
	.origin = (Vector2){.x = _x, .y = _y}, \
	.point = (Vector2){.x = _dx, .y = _dy} \
}
static const Raycast USE_RAYCAST[][2] = {
	[DIR_DOWN] = { RAYCAST(1.5f, size.y, 0, 4), RAYCAST(size.x - 1.5f, size.y, 0, 4) },
	[DIR_UP] = { RAYCAST(1.5f, 0, 1, -4), RAYCAST(size.x - 1.5f, 0, 0, -4) },
	[DIR_LEFT] = { RAYCAST(0, 1.5f, -4, 0), RAYCAST(0, size.y - 1.5f, -4, 0) },
	[DIR_RIGHT] = { RAYCAST(size.x, 1.5f, 4, 0), RAYCAST(size.x, size.y - 1.5f, 4, 0) },
};

#undef RAYCAST


Hero hero_create() {
	Hero hero = {0};
	hero.id = entity_create_id(ENTITY_HERO);
	hero.husk.position = (Rectangle){.x = 83, .y = 83, .width = TILE_SIZE, .height = TILE_SIZE};
	hero.husk.sprite_offset = (Vector2){0};
	hero.husk.facing = DIR_DOWN;
	Rectangle rect = (Rectangle){
		.x = hero.husk.position.x,
		.y = hero.husk.position.y,
		.width = size.x,
		.height = size.y,
	};
	hero.collider = collider_create(hero.id, rect, COLLISION_LAYER_PLAYER);
	hero.husk.animation.palette = HERO_PALETTE_GREEN;
	hero.husk.sword_collider = collider_create(hero.id, (Rectangle){0}, COLLISION_LAYER_SWORD);
	// collider_set_debug(hero.husk.sword_collider, true);
	// collider_set_debug(hero.collider, true);
	return hero;
}

static void hero_update_animation(HeroHusk* hero, float dt);

static inline bool hero_can_walk(Hero* hero) {
	return !hero->husk.swinging;
}


#define TO_TILE(pos) pos / TILE_SIZE


static Vector2 hero_walk(Hero* hero, float dt);
static void handle_use(Hero* hero);
static void handle_touch_use(Hero* hero);
inline static bool hero_can_swing(Hero* hero);


void hero_update(Hero* hero) {
	assert(hero);
	float dt = GetFrameTime();
	Vector2 input = {0};
	if (!hero->husk.falling)
		input = hero_walk(hero, dt);

	if (hero_can_swing(hero) && IsKeyPressed(INPUT_SWING)) {
		hero->husk.swinging = true;
		hero->husk.swing_tick = 0.f;
		sound_play(SOUND_SWORD);
		message_send_action((MessageAction){
			.action = ACTION_SWING, 
			.x = hero->husk.position.x,
			.y = hero->husk.position.y,
		});
		collider_set_enabled(hero->husk.sword_collider, true);
	}

	if (!hero->husk.falling && IsKeyPressed(INPUT_USE))
		handle_use(hero);

	hero->husk.animation.is_moving = fabs(input.x) + fabs(input.y) > 0.1f;
	hero_husk_update(&hero->husk);

	if (!hero->husk.falling && hero->falling) {
		hero->husk.position.x = TILE_SIZE * hero->safe_position_x + size.x / 2;
		hero->husk.position.y = TILE_SIZE * hero->safe_position_y + size.y / 2;
		collider_set_position(
			hero->collider, 
			(Vector2){.x=hero->husk.position.x, .y=hero->husk.position.y}
		);
		hero->falling = false;
		message_send_action((MessageAction){
			.action = ACTION_SET_POSITION,
			.x = hero->husk.position.x,
			.y = hero->husk.position.y,
		});
	}
}

inline static bool hero_can_swing(Hero* hero) {
	return !hero->husk.swinging && !hero->husk.falling;
}

static void hero_fall(Hero* hero, int tile_x, int tile_y) {
	hero->husk.fall_tick = 0.f;
	hero->husk.falling = true;
	hero->falling = true;
	hero->husk.position.x = tile_x * TILE_SIZE + (size.x / 2.f);
	hero->husk.position.y = tile_y * TILE_SIZE + (size.y / 2.f);
	sound_play(SOUND_FALL);
	collider_set_position(
		hero->collider, 
		(Vector2){.x=hero->husk.position.x, .y=hero->husk.position.y}
	);
	message_send_action((MessageAction){
		.action = ACTION_FALL,
		.x = hero->husk.position.x,
		.y = hero->husk.position.y,
	});
}

static inline bool is_position_hole(float x, float y) {
        int tile_x = ((int)x) / TILE_SIZE;
        int tile_y = ((int)y) / TILE_SIZE;
        Tile tile = map_get_tile(&game.map, tile_x, tile_y);
	return tile_is_hole(tile);
}

static void hole_slipping(Hero* hero, Vector2* velocity, int tile_x, int tile_y) {
	static const float MARGIN = 0.5f;
	bool tl = is_position_hole(
		hero->husk.position.x + MARGIN,
		hero->husk.position.y + MARGIN
	);
	bool br = is_position_hole(
		hero->husk.position.x + size.x - MARGIN,
		hero->husk.position.y + size.y - MARGIN
	);
	if (tl && br)
		hero_fall(hero, tile_x, tile_y);

	Vector2 delta = {
		.x = tile_x * TILE_SIZE + size.x / 2,
		.y = tile_y * TILE_SIZE + size.y / 2,
	};
	delta.x -= hero->husk.position.x;
	delta.y -= hero->husk.position.y;
	*velocity = Vector2Add(*velocity, Vector2Scale(Vector2Normalize(delta), 0.1));
}



static Vector2 hero_walk(Hero* hero, float dt) {
	if (!hero_can_walk(hero))
		return (Vector2){0};
	Vector2 input = {0};
	if (IsKeyDown(INPUT_LEFT)) {
		input.x = -1;
		hero->husk.facing = DIR_LEFT;
	}
	if (IsKeyDown(INPUT_RIGHT)) {
		input.x = 1;
		hero->husk.facing = DIR_RIGHT;
	}
	if (IsKeyDown(INPUT_DOWN)) {
		input.y = 1;
		hero->husk.facing = DIR_DOWN;
	}
	if (IsKeyDown(INPUT_UP)) {
		input.y = -1;
		hero->husk.facing = DIR_UP;
	}
	Vector2 velocity = Vector2Scale(Vector2Normalize(input), HERO_SPEED * dt);

        int old_tile_x = ((size.x / 2) + (int)hero->husk.position.x) / TILE_SIZE;
        int old_tile_y = ((size.y / 2) + (int)hero->husk.position.y) / TILE_SIZE;
        Tile old_tile = map_get_tile(&game.map, old_tile_x, old_tile_y);
	if (tile_is_hole(old_tile)) {
		hole_slipping(hero, &velocity, old_tile_x, old_tile_y);
	}

	collider_move(hero->collider, (Vector2){.x = velocity.x});
	Vector2 new_position = collider_move(hero->collider, (Vector2){.y = velocity.y});
	
	hero->husk.position.x = new_position.x;
	hero->husk.position.y = new_position.y;
	hero->room_x = (ROOM_OFFSET.x + new_position.x) / TILE_SIZE / ROOM_WIDTH;
	hero->room_y = (ROOM_OFFSET.y + new_position.y) / TILE_SIZE / ROOM_HEIGHT;

        int tile_x = ((size.x / 2) + (int)new_position.x) / TILE_SIZE;
        int tile_y = ((size.y / 2) + (int)new_position.y) / TILE_SIZE;
        Tile current_tile = map_get_tile(&game.map, tile_x, tile_y);
	hero->is_safe_position = false;
	if (tile_is_safe(current_tile)) {
		hero->safe_position_x = tile_x;
		hero->safe_position_y = tile_y;
		hero->is_safe_position = true;
	}


//	printf("Tile@(%d, %d)\n", hero->safe_position_x, hero->safe_position_y);
//	game.camera.target.x = hero->room_x * ROOM_WIDTH * TILE_SIZE;
//	game.camera.target.y = hero->room_y * ROOM_HEIGHT * TILE_SIZE;
	if (input.x != 0 || input.y != 0)
		handle_touch_use(hero);
	return input;
}



static inline bool raycast_hit(Hero* hero, CollisionLayer layer, RaycastHitResult* result) {
	bool found = false;
	size_t cast_amount = sizeof(USE_RAYCAST[0]) / sizeof(USE_RAYCAST[0][0]);
	for (size_t i = 0; i < cast_amount && !found; i++) {
		Raycast cast = USE_RAYCAST[hero->husk.facing][i];
		cast.origin = Vector2Add(cast.origin, AS_VEC(hero->husk.position));
		found = collider_raycast_hit(cast, layer, result);
	}
	return found;
}


static void handle_touch_use(Hero* hero) {
	static const CollisionLayer layer = COLLISION_LAYER_DOOR;
	RaycastHitResult result = {0};
	if (!raycast_hit(hero, layer, &result))
		return;
	switch (result.parent.type) {
	case ENTITY_DOOR:
		door_on_interact(entity_lookup(result.parent), hero);
		return;
	default:
		LOG("Tried to touch '%s' %zu\n", ENTITY_TYPE_TO_STR[result.parent.type], result.collider);
		return;
	}
}


static void handle_use(Hero* hero) {
	static const CollisionLayer layer = COLLISION_LAYER_OWL | COLLISION_LAYER_CHEST;
	RaycastHitResult result = {0};
	if (!raycast_hit(hero, layer, &result)) {
		LOG("No interactions.\n");
		return;
	}
	switch (result.parent.type) {
	case ENTITY_OWL:
		owl_on_interact(entity_lookup(result.parent));
		return;
	case ENTITY_CHEST:
		chest_on_interact(entity_lookup(result.parent), hero);
		return;
	default:
		LOG("Interact with '%s'\n", ENTITY_TYPE_TO_STR[result.parent.type]);
		return;
	}
}


static inline int swing_frame(HeroHusk* hero) {
	int frame = (hero->swing_tick / SWING_DELAY) * 4;
	if (frame == 3)
		frame = 2;
	return frame;
}

static void on_swing_collision(EntityId other, ColliderId other_collider, void* data) {
	HeroHusk* hero = data;
	void* entity = entity_lookup(other);
	if (entity == NULL)
		return;
	switch (other.type) {
	case ENTITY_SWITCH: switch_on_hit(entity, SWING_DELAY - hero->swing_tick); break;
	default:
		break;
	}
}

void hero_husk_update(HeroHusk *hero) {
	assert(hero);
	float dt = GetFrameTime();
	if (hero->falling) {
		hero->fall_tick += dt;
		hero->falling = hero->fall_tick < FALL_DELAY;
	}
	if (hero->swinging) {
		hero->swing_tick += dt;
		hero->swinging = hero->swing_tick < SWING_DELAY;
		if (hero->swinging) {
			int frame = swing_frame(hero);
			Rectangle sword_area = {
				.x = hero->position.x + SWORD_AREA[hero->facing][frame].x,
				.y = hero->position.y + SWORD_AREA[hero->facing][frame].y,
				SWORD_AREA[hero->facing][frame].width,
				SWORD_AREA[hero->facing][frame].height,
			};
			collider_set_area(hero->sword_collider, sword_area);
			collider_query_hit(hero->sword_collider, on_swing_collision, hero);
		} else {
			collider_set_enabled(hero->sword_collider, false);
		}
	}
	hero_update_animation(hero, dt);
}



static inline void hero_update_animation(HeroHusk* hero, float dt) {
	if (hero->falling) {
		hero->animation.state = HERO_STATE_FALLING;
		return;
	}
	if (hero->swinging) {
		hero->animation.state = HERO_STATE_SWING;
		return;
	}

	hero->animation.state = hero->animation.is_moving ? HERO_STATE_WALK : HERO_STATE_IDLE;
	if (hero->animation.is_moving) {
		hero->animation.flip_tick += dt;
		hero->animation.walk_tick += dt;
		if (hero->animation.flip_tick > FLIP_DELAY) {
			hero->animation.flipped = !hero->animation.flipped;
			hero->animation.flip_tick -= FLIP_DELAY;
		}
		if (hero->animation.walk_tick > WALK_DELAY) {
			hero->animation.walk_tick -= WALK_DELAY;
		}
	} else {
		hero->animation.flip_tick = FLIP_DELAY;
		hero->animation.walk_tick = WALK_DELAY / 0.15f;
	}
}


static inline Rectangle hero_sprite_idle(HeroHusk* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return hero->animation.flipped ? RECTF(0, 0) : RECT(0, 0);
	case DIR_UP:
		return hero->animation.flipped ? RECTF(1, 0) : RECT(1, 0);
	case DIR_LEFT:
		return RECT(2, 0);
	case DIR_RIGHT:
		return RECTF(2, 0);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


static inline Rectangle hero_sprite_walk(HeroHusk* hero) {
	switch (hero->facing) {
	case DIR_DOWN:
		return hero->animation.flipped ? RECTF(0, 0) : RECT(0, 0);
	case DIR_UP:
		return hero->animation.flipped ? RECTF(1, 0) : RECT(1, 0);
	case DIR_LEFT:
		return hero->animation.walk_tick < WALK_DELAY / 2.f ? RECT(2, 0) : RECT(3, 0);
	case DIR_RIGHT:
		return hero->animation.walk_tick < WALK_DELAY / 2.f ? RECTF(2, 0) : RECTF(3, 0);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}


static inline Rectangle hero_sprite_swing(HeroHusk* hero) {
	static const int SWING_REACH = 4;
	int frame = (hero->swing_tick / SWING_DELAY) * 4;
	switch (hero->facing) {
	case DIR_DOWN:
		hero->sprite_offset.y = frame >= 2 ? SWING_REACH : 0;
		return frame == 0 ? RECT(0, 1) : RECT(1, 1);
	case DIR_UP:
		hero->sprite_offset.y = frame >= 2 ? -SWING_REACH : 0;
		return frame == 0 ? RECT(2, 1) : RECT(3, 1);
	case DIR_LEFT:
		hero->sprite_offset.x = frame >= 2 ? -SWING_REACH : 0;
		return frame == 0 ? RECT(4, 1) : RECT(5, 1);
	case DIR_RIGHT:
		hero->sprite_offset.x = frame >= 2 ? SWING_REACH : 0;
		return frame == 0 ? RECTF(4, 1) : RECTF(5, 1);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}

static inline Rectangle hero_sprite_falling(HeroHusk* hero) {
	int idx = (hero->fall_tick * 4) / FALL_DELAY;
	return FALL_SPRITES[idx];
}


static Rectangle hero_sprite(HeroHusk* hero) {
	switch (hero->animation.state) {
	case HERO_STATE_IDLE:
		return hero_sprite_idle(hero);
	case HERO_STATE_FALLING:
		return hero_sprite_falling(hero);
	case HERO_STATE_WALK:
		return hero_sprite_walk(hero);
	case HERO_STATE_SWING:
		return hero_sprite_swing(hero);
	default:
		assert(0);
		return NONE_SPRITE;
	}
}

static inline void sword_render(HeroHusk* hero) {
	if (!hero->swinging)
		return;
	int frame = swing_frame(hero);

	Rectangle sprite = SWORD_SPRITES[hero->facing][frame];
	Vector2 offset = SWORD_OFFSETS[hero->facing][frame];
	Rectangle position = hero->position;
	position.x += offset.x + hero->sprite_offset.x;
	position.y += offset.y + hero->sprite_offset.y;
	DrawTexturePro(
		textures.sword,
		sprite,
		position,
		origin,
		0,
		WHITE
	);
}


void hero_husk_render(HeroHusk* hero) {
	assert(hero->position.width > 0 && hero->position.height > 0);

	hero->sprite_offset.x = 0;
	hero->sprite_offset.y = 0;
	Rectangle sprite = hero_sprite(hero);
	Rectangle position = hero->position;
	position.x += hero->sprite_offset.x;
	position.y += hero->sprite_offset.y;
	
	DrawTexturePro(
		textures.hero_palettes[hero->animation.palette], 
		sprite,
		position,
		origin,
		0,
		WHITE
	);
	sword_render(hero);
	// draw_point(hero->position.x, hero->position.y, GREEN);
	
	/*
	size_t cast_amount = sizeof(USE_RAYCAST[0]) / sizeof(USE_RAYCAST[0][0]);
	for (size_t i = 0; i < cast_amount; i++) {
		Raycast cast = USE_RAYCAST[hero->facing][i];
		cast.origin = Vector2Add(cast.origin, AS_VEC(hero->position));
		Vector2 to = Vector2Add(cast.origin, cast.point);
		DrawLineEx(cast.origin, to, 1, RED);
	}
	*/
}

void hero_render(Hero* hero) {
	hero_husk_render(&hero->husk);

        // int tile_x = ((size.x / 2) + (int)hero->husk.position.x) / TILE_SIZE;
        // int tile_y = ((size.y / 2) + (int)hero->husk.position.y) / TILE_SIZE;
	// draw_point(
	// 	tile_x * TILE_SIZE + (TILE_SIZE / 2),
	// 	tile_y * TILE_SIZE + (TILE_SIZE / 2),
	// 	RED
	// );
}
