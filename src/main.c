#include "game.h"
#include "map.h"
#include "server.h"

#include <raylib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);
	if (argc > 1 && strcmp(argv[1], "--server") == 0)
		return server_main();

	printf("Map size: %lu", sizeof(Map));
	game_create();
	while(game_is_running()) {
		game_update();
		game_render();
	}
	game_destroy();
}

