#include "game.h"
#include "map.h"
#include "server.h"

#include <raylib.h>
#include <stdio.h>
#include <string.h>


static void late_init() {
	printf("Map size: %lu\n", sizeof(Map));
}


int main(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);
	if (argc > 1 && strcmp(argv[1], "--server") == 0)
		return server_main();

	game_create();
	late_init();
	while(game_is_running()) {
		game_update();
		game_render();
	}
	game_destroy();
}

