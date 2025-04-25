#include "client.h"
#include "game.h"
#include "message.h"
#include "log.h"
#include "network_hero.h"

#include <assert.h>
#include <fcntl.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h>

#define LOG_HEADER "NETWORK"

Client client = {0};

const double NETWORK_SYNC_DELAY = 0.1;
static const int MAX_MESSSAGES = 20;

static inline void message_send(Message message) {
	fflush(stdout);
	message.sender = client.uid;
	sendto(
		client.socketfd, 
		&message, sizeof(message),
		0,
		(const struct sockaddr*) &client.servaddr, sizeof(client.servaddr)
	);
	fflush(stdout);
}


static inline int message_receive(Message* message) {
	socklen_t len;
	int n = recvfrom(
		client.socketfd, 
		message, sizeof(*message),
		0,
		(struct sockaddr *)&client.servaddr, &len
	);
	return n;
}


static inline bool find_network_hero(size_t owner, NetworkHero** hero) {
	for (size_t i = 0; i < game.network_hero_count; i++) {
		if (game.network_heroes[i].owner != owner)
			continue;
		*hero = &game.network_heroes[i];
		return true;
	}
	assert(MAX_HERO_HUSKS > game.network_hero_count);
	*hero = &game.network_heroes[game.network_hero_count++];
	(*hero)->owner = owner;
	return false;
}


static void handle_sync_state(size_t sender, MessageState* state) {
	NetworkHero* network_hero = NULL;
	bool found = find_network_hero(sender, &network_hero);
	assert(network_hero != NULL);
	if (!found)
		network_hero_init(network_hero, &state->network_hero);
	hero_husk_get_state(&network_hero->husk, &network_hero->previous);
	network_hero_handle_sync(network_hero, &state->network_hero);
}


static void handle_action(size_t sender, MessageAction* action) {
	NetworkHero* network_hero = NULL;
	find_network_hero(sender, &network_hero);
	assert(network_hero != NULL);
	switch (action->action) {
	case ACTION_SWING:
		network_hero_handle_action(network_hero, action->x, action->y);
			break;
		default:
			assert(0);
	}
}


static void handle_assign_uid(size_t uid) {
	LOG("Assigning UID: %zu\n", uid);
	client.uid = uid;
	client.status = CLIENT_STATUS_READY;
	game.hero.husk.animation.palette = (uid - 1) % HERO_PALETTE_SIZE;
}


static void handle_server_message(Message* message) {
	switch (message->tag) {
		case TAG_ASSIGN_UID:
			handle_assign_uid(message->data.uid);
			break;
		case TAG_SYNC_STATE:
			handle_sync_state(message->sender, &message->data.state);
			break;
		case TAG_ACTION:
			handle_action(message->sender, &message->data.action);
		break;
	case TAG_CONNECT:
		LOG("Unexpected message type: TAG_CONNECT\n");
		break;
	case TAG_DISCONNECT:
		LOG("Unexpected message type: TAG_DISCONNECT\n");
		break;
	default:
		LOG("Unknown message type: %d\n", message->tag);
		break;
	}
}


void network_client_create() {
	if ( (client.socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed.");
		exit(EXIT_FAILURE);
	}
	memset(&client.servaddr, 0, sizeof(client.servaddr));
	client.servaddr.sin_family = AF_INET;
	client.servaddr.sin_port = htons(PORT);
	client.servaddr.sin_addr.s_addr = INADDR_ANY;

	fcntl(client.socketfd, F_SETFL, O_NONBLOCK);

	client.status = CLIENT_STATUS_CONNECTED;
	message_send((Message){.tag = TAG_CONNECT });
	LOG("Client connected...\n");
}


void network_client_destroy() {
	message_send((Message){.tag = TAG_DISCONNECT });
	if (client.status > CLIENT_STATUS_DISCONNECTED)
		close(client.socketfd);
	client.status = CLIENT_STATUS_DISCONNECTED;
}


static inline Message message_create_state_sync() {
	Message message = {.tag = TAG_SYNC_STATE};
	hero_husk_get_state(&game.hero.husk, &message.data.state.network_hero);
	return message;
}


void message_send_action(MessageAction action) {
	Message message = {.tag = TAG_ACTION};
	message.data.action = action;
	message.data.action.owner = client.uid;
	message_send(message);
}


void network_client_update() {
	if (client.status == CLIENT_STATUS_DISCONNECTED)
		return;
	int n;
	Message message = {0};
	int message_c = 0;
	while (MAX_MESSSAGES > message_c++) {
		n = message_receive(&message);
		if (n <= 0)
			break;
		handle_server_message(&message);
	}
	if (client.status != CLIENT_STATUS_READY)
		return;

	double time = GetTime();
	if (client.last_sync + NETWORK_SYNC_DELAY < time) {
		client.last_sync = time;
		message_send(message_create_state_sync());
	}
}

