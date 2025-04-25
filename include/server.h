#ifndef SERVER_H
#define SERVER_H

#include <time.h>
#include <netinet/in.h>
#include <stdbool.h>


#define PORT 3000
#define MAXCLIENT 32
#define TIMEOUT 10

typedef struct {
	int socketfd;
	struct sockaddr_in address;
} Server;


typedef struct {
	bool alive;
	time_t last_active;
	int uid;
	struct sockaddr_in cliaddr;
} Client;


typedef struct {
	Server server;
	time_t now;
	size_t last_uid;
} ServerState;


int server_main();


#endif // SERVER_H
