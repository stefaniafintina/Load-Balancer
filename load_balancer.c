/* Copyright 2023 <Fintina Stefania 314CA> */
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "load_balancer.h"

struct load_balancer {
    server_memory **server_ht;
    array_info *v;
    int v_size;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;
    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer(struct load_balancer *lb) {
    lb = malloc(sizeof(load_balancer));
    DIE(lb == NULL, "eroare");
    lb->server_ht = malloc(99999 * sizeof(server_memory *));
    DIE(lb->server_ht == NULL, "eroare");
    for (int i = 0; i < 99999; i++) {
        lb->server_ht[i] = malloc(sizeof(server_memory));
        DIE(lb->server_ht[i] == NULL, "eroare");
        lb->server_ht[i]->ht = ht_create(10, hash_function_string,
		compare_function_strings);
    }
    lb->v_size = 0;
    return lb;
}

void loader_add_server(load_balancer *main, int server_id) {
    if (main->v_size == 0) {
        main->v_size = 3;
        main->v = malloc(main->v_size * sizeof(array_info));
        DIE(main->v == NULL, "eroare");
    } else {
        main->v_size += 3;
        array_info *aux;
        aux = realloc(main->v, main->v_size * sizeof(array_info));
        DIE(aux == NULL, "eroare");
        main->v = aux;
    }
    unsigned int sid = server_id;
    main->v[main->v_size - 1].server_id = server_id;
    main->v[main->v_size - 2].server_id = server_id;
    main->v[main->v_size - 3].server_id = server_id;
    main->v[main->v_size - 1].hash_val = hash_function_servers(&sid);
    sid = sid + 100000;
    main->v[main->v_size - 2].hash_val = hash_function_servers(&sid);
    sid = sid + 100000;
    main->v[main->v_size - 3].hash_val = hash_function_servers(&sid);

    int ok = 0;

    while (ok == 0) {
        ok = 1;
        for (int i = 0; i < main->v_size - 1; i++) {
            if (main->v[i].hash_val > main->v[i + 1].hash_val) {
                array_info auxi = main->v[i];
                main->v[i] = main->v[i + 1];
                main->v[i + 1] = auxi;
                ok = 0;
            }
        }
    }

    for (int i = 0; i < main->v_size && main->v_size > 3; i++) {
        if (main->v[i].server_id == server_id) {
            int k = (i + 1) % main->v_size;
            while (main->v[k].server_id == server_id) {
                k++;
                k %= main->v_size;
            }
            int index = main->v[k].server_id;
            unsigned int j;
            for (j = 0; j < ht_get_hmax(main->server_ht[index]->ht); j++) {
                ll_node_t *curr = main->server_ht[index]
                                        ->ht->buckets[j]->head, *aux;
                while (curr) {
                    aux = curr->next;
                    int new_store = b_search(main, ((info *)curr->data)->key);
                    if (new_store != index) {
                        server_store(main->server_ht[new_store],
                                        ((info *)curr->data)->key,
                                        ((info *)curr->data)->value);
                        ht_remove_entry(main->server_ht[index]->ht,
                                        ((info *)curr->data)->key);
                    }
                    curr = aux;
                }
            }
        }
    }
}

int b_search(load_balancer *main, char *key) {
    int left, right, m;
    unsigned int hash_key = hash_function_key(key);
    left = 0;
    right = main->v_size - 1;
    m = (left + right)/2;
    int id = main->v[0].server_id;
    while (left <= right) {
        if (hash_key > main->v[m].hash_val) {
            left = m + 1;
            m = (left + right)/2;
        } else if (hash_key < main->v[m].hash_val) {
            id = main->v[m].server_id;
            right = m - 1;
            m = (left + right)/2;

        } else if (hash_key == main->v[m].hash_val) {
            return id;
        }
    }
    return id;
}
void loader_remove_server(load_balancer *main, int server_id) {
    for (int i = 0; i < main->v_size - 1; i++) {
        if (main->v[i].server_id == server_id) {
            for (int j = i; j < main->v_size - 1; j++)
                main->v[j] = main->v[j + 1];
            i--;
            main->v_size--;
        }
    }
    if (main->v[main->v_size - 1].server_id == server_id)
        main->v_size--;
    unsigned int j;
    for (j = 0; j < ht_get_hmax(main->server_ht[server_id]->ht); j++) {
        ll_node_t *curr = main->server_ht[server_id]
                            ->ht->buckets[j]->head, *aux;
        while (curr) {
            aux = curr->next;
            int new_store = b_search(main, ((info *)curr->data)->key);
            server_store(main->server_ht[new_store],
                            ((info *)curr->data)->key,
                            ((info *)curr->data)->value);
            ht_remove_entry(main->server_ht[server_id]->ht,
                                ((info *)curr->data)->key);

            curr = aux;
        }
    }

    int ok = 0;

    while (ok == 0) {
        ok = 1;
        for (int i = 0; i < main->v_size - 1; i++) {
            if (main->v[i].hash_val > main->v[i + 1].hash_val) {
                array_info auxi = main->v[i];
                main->v[i] = main->v[i + 1];
                main->v[i + 1] = auxi;
                ok = 0;
            }
        }
    }
}

void loader_store(load_balancer *main, char *key,
                  char *value, int *server_id) {
    int id = b_search(main, key);
    *server_id = id;
    server_store(main->server_ht[id], key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    int id = b_search(main, key);
    *server_id = id;
    return server_retrieve(main->server_ht[*server_id], key);
}

void free_load_balancer(load_balancer *main) {
    free(main->v);
    for (int i = 0; i < 99999; i++) {
        ht_free(main->server_ht[i]->ht);
        free(main->server_ht[i]);
    }
    free(main->server_ht);
    free(main);
}
