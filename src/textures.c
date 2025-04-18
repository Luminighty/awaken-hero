#include "textures.h"
#include "config.h"
#include <raylib.h>

Textures textures = {0};


void textures_create() {
	textures.render_target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	textures.hero = LoadTexture("./assets/link.png");
	textures.tileset = LoadTexture("./assets/tileset.png");
}

void textures_destroy() {
	UnloadRenderTexture(textures.render_target);
	UnloadTexture(textures.hero);
	UnloadTexture(textures.tileset);
}
