#ifndef TEXTURES_H
#define TEXTURES_H

#include <raylib.h>


typedef struct {
	RenderTexture2D render_target;
	Texture2D hero;
	Texture2D tileset;
} Textures;

extern Textures textures;

void textures_create();
void textures_destroy();

#endif // TEXTURES_H
