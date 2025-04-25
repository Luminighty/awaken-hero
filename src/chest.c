#include "chest.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "textures.h"
#include "log.h"
#include <assert.h>

#define LOG_HEADER "CHEST"

#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle CHEST_SPRITE = TILE(0, 5);
static Rectangle CHEST_OPENED_SPRITE = TILE(1, 5);


Chest chest_create(int x, int y, ChestLoot loot) {
	Chest chest = {0};
	chest.loot = loot;
	chest.id = entity_create_id(ENTITY_CHEST);
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	chest.collider = collider_create(chest.id, rect, COLLISION_LAYER_CHEST);
	chest.position = rect;
	// collider_set_debug(chest.collider, true);
	return chest;
}


void chest_destroy(Chest* chest) {
	collider_destroy(chest->collider);
}


void chest_render(Chest* chest) {
	DrawTexturePro(
		textures.tileset,
		chest->is_opened ? CHEST_OPENED_SPRITE : CHEST_SPRITE,
		chest->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}


void chest_on_interact(Chest* chest, Hero* hero) {
	assert(chest);
	if (chest->is_opened) {
		LOG("Already opened\n");
		return;
	}
	chest->is_opened = true;
	switch (chest->loot) {
	case CHEST_LOOT_NOTHING:
		LOG("Opened nothing.\n");
		break;
	case CHEST_LOOT_RANDOM:
		LOG("Opened random.\n");
		break;
	case CHEST_LOOT_KEY:
		hero->inventory.keys++;
		LOG("Got key: %d\n", hero->inventory.keys);
		break;
	case CHEST_LOOT_BOSSKEY:
		hero->inventory.boss_key = true;
		LOG("Opened bosskey.\n");
		break;
	default:
		assert(0);
	}
}

