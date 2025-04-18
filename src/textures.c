#include "textures.h"
#include <raylib.h>

Textures textures = {0};


void textures_create() {
	textures.hero = LoadTexture("./assets/link.png");
	textures.tileset = LoadTexture("./assets/tileset.png");
}

void textures_destroy() {
	UnloadTexture(textures.hero);
	UnloadTexture(textures.tileset);
}
