#include "server.h"

#include "message.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static int socketfd;


static Server server_create() {
	Server server = {0};
	if ( (server.socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed.");
		exit(EXIT_FAILURE);
	}
	server.address.sin_family = AF_INET;
	server.address.sin_addr.s_addr = INADDR_ANY;
	server.address.sin_port = htons(PORT);

	if ( bind(server.socketfd, (const struct sockaddr *)&server.address, sizeof(server.address)) < 0 ) {
		perror("bind failed.");
		exit(EXIT_FAILURE);
	}
	socketfd = server.socketfd;

	printf("Server ready.\n");
	return server;
}


static Client* find_or_insert_client(ServerState* state, Client* clients, size_t* client_c, Client* item) {
	assert(item);
	size_t free_slot = *client_c;
	size_t last_alive = 0;
	for (size_t i = 0; i < *client_c; i++) {
		if (!clients[i].alive) {
			if (free_slot > i)
				free_slot = i;
			continue;
		}
		last_alive = i;
		bool address_match = memcmp(&clients[i].cliaddr.sin_addr, &item->cliaddr.sin_addr, sizeof(item->cliaddr.sin_addr)) == 0;
		bool port_match = memcmp(&clients[i].cliaddr.sin_port, &item->cliaddr.sin_port, sizeof(item->cliaddr.sin_port)) == 0;
		if (port_match && address_match)
			return &clients[i];
	}
	*client_c = last_alive + 1;
	if (free_slot == *client_c) {
		assert(MAXCLIENT > *client_c);
		*client_c += 1;
	}
	memcpy(&clients[free_slot], item, sizeof(*item));
	clients[free_slot].uid = ++state->last_uid;

	Message message_init = {0};
	message_init.tag = TAG_ASSIGN_UID;
	message_init.data.uid = clients[free_slot].uid;
	printf("New client joined!\n");
	printf(
		"Setting uid: tag: %s, uid: %zu len: %lu\n", 
		message_tag_labels[message_init.tag], 
		message_init.data.uid, 
		sizeof(message_init)
	);
	int res = sendto(
		state->server.socketfd,
		&message_init, sizeof(message_init),
		0,
		(const struct sockaddr *)&clients[free_slot].cliaddr,
		sizeof(clients[free_slot].cliaddr)
	);
	if (res < 0) {
		perror("sendto error on client connection");
	} else if (res != sizeof(message_init)) {
		fprintf(stderr, "sendto sent %d of %zu bytes", res, sizeof(message_init));
	}
	clients[free_slot].alive = true;

	return &clients[free_slot];
}


static void broadcast(ServerState* state, Client* clients, size_t client_c, Client* sender, Message* message, int message_len) {
	message->sender = sender->uid;
	for (size_t i = 0; i < client_c; i++) {
		if (clients[i].uid == sender->uid)
			continue;
		if (!clients[i].alive)
			continue;
		if (clients->last_active + TIMEOUT < state->now)
			clients->alive = false;
		sendto(
			state->server.socketfd,
			(const char*)message, message_len,
			MSG_CONFIRM,
			(const struct sockaddr *)&clients[i].cliaddr, sizeof(sender->cliaddr)
		);
	}
}


static void handle_sigint(int sig) {
    printf("\nCaught signal %d, closing server socket...\n", sig);
    close(socketfd);
    exit(0);
}


int server_main() {
	printf("Starting server...\n");
	ServerState state = {0};
	state.server = server_create();
	signal(SIGINT, handle_sigint);
	Message message;

	Client clients[MAXCLIENT] = {0};
	size_t client_c = 0;

	Client current = {0};
	current.alive = true;
	socklen_t len = sizeof(current.cliaddr);
	state.now = time(NULL);
	while (1) {
		fflush(stdout);
		int n = recvfrom(
			state.server.socketfd,
			&message, sizeof(Message),
			0,
			(struct sockaddr*)&current.cliaddr, &len
		);
		state.now = time(NULL);
		Client* actual = find_or_insert_client(&state, clients, &client_c, &current);
		actual->last_active = state.now;

		fflush(stdout);

		// TODO: Send out a Connection received message to clients?
		if (message.tag == TAG_CONNECT)
			continue;

		// TODO: Send out a disconnected message to clients?
		if (message.tag == TAG_DISCONNECT) {
			actual->alive = false;
			continue;
		}

		broadcast(&state, clients, client_c, actual, &message, n);
	}
	close(state.server.socketfd);
	return 0;
}

