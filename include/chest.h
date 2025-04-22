#ifndef CHEST_H
#define CHEST_H

#include "collision.h"
#include "hero.h"
#include <raylib.h>


typedef enum {
	CHEST_LOOT_NOTHING,
	CHEST_LOOT_RANDOM,
	CHEST_LOOT_KEY,
	CHEST_LOOT_BOSSKEY,
} ChestLoot;

typedef struct {
	EntityId id;
	Rectangle position;
	ColliderId collider;
	bool is_opened;
	ChestLoot loot;
} Chest;

Chest chest_create(int x, int y, ChestLoot loot);
void chest_destroy(Chest* chest);
void chest_render(Chest* chest);
void chest_on_interact(Chest* chest, Hero* hero);

#endif // CHEST_H
