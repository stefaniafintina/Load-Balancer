/* Copyright 2023 <Fintina Stefania 314CA> */
#include <stdlib.h>
#include <string.h>

#include "server.h"

server_memory *init_server_memory(struct server_memory *server)
{
	server = malloc(sizeof(server_memory));
	DIE(server == NULL, "eroare");
	server->ht = ht_create(1024, hash_function_string,
		compare_function_strings);
	DIE(server->ht == NULL, "eroare");
	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	ht_put(server->ht, key, 128, value, 65536);
}

char *server_retrieve(server_memory *server, char *key) {
	return ht_get(server->ht, key);
}

void server_remove(server_memory *server, char *key) {
	ht_remove_entry(server->ht, key);
}

void free_server_memory(server_memory *server) {
	ht_free(server->ht);
	free(server);
}
