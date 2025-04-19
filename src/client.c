#include "client.h"
#include "game.h"
#include "message.h"

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

#define LOG(...) printf("[NETWORK] " __VA_ARGS__); fflush(stdout)

Client client = {0};

static const double SYNC_DELAY = 0.2;
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


static void handle_sync_state(size_t sender, MessageState* state) {
	bool found = false;
	for (size_t i = 0; i < game.hero_husk_count && !found; i++) {
		if (game.hero_husks[i].owner != sender)
			continue;
		game.hero_husks[i] = state->husk;
		found = true;
	}
	if (!found) {
		assert(MAX_HERO_HUSKS > game.hero_husk_count);
		game.hero_husks[game.hero_husk_count++] = state->husk;
	}
}

static void handle_action(size_t sender, MessageAction* action) {
	// TODO: Implement action syncing
}

static void handle_assign_uid(size_t uid) {
	client.uid = uid;
	client.status = CLIENT_STATUS_READY;
	game.hero.husk.animation.palette = (uid - 1) % HERO_PALETTE_SIZE;
	game.hero.husk.owner = uid;
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


static inline Message message_state() {
	Message message = {.tag = TAG_SYNC_STATE};
	message.data.state.husk = game.hero.husk;
	return message;
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
	LOG("Client connected...");
}


void network_client_destroy() {
	message_send((Message){.tag = TAG_DISCONNECT });
	if (client.status > CLIENT_STATUS_DISCONNECTED)
		close(client.socketfd);
	client.status = CLIENT_STATUS_DISCONNECTED;
}


static inline Message message_create_state_sync() {
	Message message = {.tag = TAG_SYNC_STATE};
	message.data.state.husk = game.hero.husk;
	return message;
}

void network_client_update() {
	if (client.status == CLIENT_STATUS_DISCONNECTED)
		return;
	int n;
	Message message;
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
	if (client.last_sync + SYNC_DELAY < time) {
		client.last_sync = time;
		message_send(message_create_state_sync());
	}
}

