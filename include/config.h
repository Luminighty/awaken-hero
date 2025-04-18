#ifndef CONFIG_H
#define CONFIG_H


#include <raylib.h>
#define TILE_SIZE 16

#define ROOM_WIDTH 11
#define ROOM_HEIGHT 8
#define ROOM_SIZE (ROOM_WIDTH * ROOM_HEIGHT)

#define MAP_WIDTH 8
#define MAP_HEIGHT 8

#define SCREEN_WIDTH (TILE_SIZE * ROOM_WIDTH)
#define SCREEN_HEIGHT (TILE_SIZE * ROOM_HEIGHT)

#define TITLE "Awaken Hero"

typedef enum {
	INPUT_DOWN = KEY_DOWN,
	INPUT_UP = KEY_UP,
	INPUT_LEFT = KEY_LEFT,
	INPUT_RIGHT = KEY_RIGHT,
} Input;


#endif // CONFIG_H
