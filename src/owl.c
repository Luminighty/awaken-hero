#include "owl.h"

#include "collision.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "map.h"
#include "textures.h"
#include <stdio.h>
#include <string.h>

#define LOG_HEADER "OWL"


#define TILE(x, y) (Rectangle){x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}
static Rectangle OWL_SPRITE = TILE(4, 3);


Owl owl_create(int x, int y) {
	Owl owl = {0};
	owl.id = entity_create_id(ENTITY_OWL);
	Rectangle rect = { .x = x, .y = y, .width = 16, .height = 16 };
	owl.collider = collider_create(owl.id, rect, COLLISION_LAYER_OWL);
	owl.position = rect;
	// collider_set_debug(owl.collider, true);
	strcpy(owl.message, "It's dangerous to go alone.");
	return owl;
}


void owl_destroy(Owl* owl) {
	collider_destroy(owl->collider);
}


void owl_render(Owl* owl) {
	DrawTexturePro(
		textures.tileset,
		OWL_SPRITE,
		owl->position,
		(Vector2){0, 0}, 0,
		WHITE
	);
}


void owl_on_interact(Owl* owl) {
	LOG("%s\n", owl->message);
}

