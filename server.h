/* Copyright 2023 <Fintina Stefania 314CA> */
#ifndef SERVER_H_
#define SERVER_H_
#include "hash.h"

typedef struct per_info {
    char key[128];
    char val[65536];
}per_info;

struct server_memory {
	hashtable_t *ht;
};

typedef struct server_memory server_memory;

server_memory *init_server_memory();

void free_server_memory(server_memory *server);

void server_store(server_memory *server, char *key, char *value);

void server_remove(server_memory *server, char *key);

char *server_retrieve(server_memory *server, char *key);

#endif /* SERVER_H_ */
