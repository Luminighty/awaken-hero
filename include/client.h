#ifndef CLIENT_H
#define CLIENT_H

#include "message.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <time.h>

#define PORT 3000

extern const double NETWORK_SYNC_DELAY;

typedef enum {
	CLIENT_STATUS_DISCONNECTED,
	CLIENT_STATUS_CONNECTED,
	CLIENT_STATUS_READY,
} ClientStatus;

typedef struct {
	size_t uid;
	double last_sync;
	bool is_state_dirty;
	ClientStatus status;
	int socketfd;
	struct sockaddr_in servaddr;
} Client;

extern Client client;


void network_client_create();
void network_client_destroy();
void network_client_update();

void message_send_action(MessageAction action);

#endif // CLIENT_H
