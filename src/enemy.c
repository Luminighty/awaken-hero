#include "enemy.h"
#include "collision.h"
#include "config.h"
#include "entity.h"
#include "textures.h"
#include <assert.h>
#include <raylib.h>

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}

Enemy enemy_create(EnemyKind kind, int x, int y) {
	Enemy enemy = {0};
	enemy.kind = kind;
	enemy.id = entity_create_id(ENTITY_ENEMY);
	Rectangle rect = { .x = x, .y = y, .width = TILE_SIZE, .height = TILE_SIZE };
	enemy.position = rect;
	enemy.collider = collider_create(enemy.id, rect, COLLISION_LAYER_ENEMY);
	return enemy;
}

Enemy bat_create(int x, int y) {
	return enemy_create(ENEMY_BAT, x, y);
						}


static inline Rectangle base_sprite(EnemyKind kind) {
	switch (kind) {
	case ENEMY_BAT:
		return TILE(0, 0);
	case ENEMY_NONE:
	case ENEMY__SIZE:
		assert(0);
	}
}

void enemy_render(Enemy* enemy) {
	Rectangle sprite = base_sprite(enemy->kind);
	static const Vector2 origin = {TILE_SIZE / 2.f, TILE_SIZE / 2.f};

	DrawTexturePro(
		textures.enemies,
		sprite,
		(Rectangle){
			.x = enemy->position.x + origin.x,
			.y = enemy->position.y + origin.y,
			.width = enemy->position.width,
			.height = enemy->position.height,
		},
		origin,
		0.0f,
		WHITE
	);
}


void enemy_update(Enemy* enemy) {
	// TODO:
}

