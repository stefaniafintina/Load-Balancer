/* Copyright 2023 <Fintina Stefania 314CA> */
#ifndef HASH_H_
#define HASH_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

typedef struct ll_node_t ll_node_t;
typedef struct linked_list_t linked_list_t;

/* Helper data structures definitions */
struct ll_node_t
{
	void* data;
	ll_node_t* next;
};

struct linked_list_t
{
	ll_node_t* head;
	unsigned int data_size;
	unsigned int size;
};

linked_list_t* ll_create(unsigned int data_size);
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n);
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);
ll_node_t* ll_remove_nth_node(linked_list_t* list, unsigned int n);
unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);



typedef struct info {
	void *key;
	void *value;
} info;

typedef struct hashtable_t hashtable_t;

struct hashtable_t {
	linked_list_t **buckets;
	unsigned int size;
	unsigned int hmax;
	unsigned int (*hash_function)(void*);
	int (*compare_function)(void*, void*);
};

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));
int ht_has_key(hashtable_t *ht, void *key);
void* ht_get(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);
unsigned int hash_function_int(void *a);
unsigned int hash_function_string(void *a);
int compare_function_strings(void *a, void *b);
int compare_function_ints(void *a, void *b);


#endif  // HASH_H_


