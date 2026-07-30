#ifndef ENEMY_H
#define ENEMY_H

#include "collision.h"


typedef enum {
	ENEMY_NONE,
	ENEMY_BAT,
	ENEMY__SIZE,
} EnemyKind;


typedef struct {
	EnemyKind kind;
	EntityId id;
	Rectangle position;
	ColliderId collider;
	int hp;
	int anim_frame;
} Enemy;


Enemy bat_create(int x, int y);
Enemy enemy_create(EnemyKind kind, int x, int y);
void enemy_render(Enemy* enemy);
void enemy_update(Enemy* enemy);

#endif // ENEMY_H
