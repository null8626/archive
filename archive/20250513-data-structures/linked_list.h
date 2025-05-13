#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdbool.h>
#include <stdlib.h>

typedef void* single_node_data_t;
typedef void(* single_node_data_free_t)(single_node_data_t);

typedef struct single_node {
  single_node_data_t data;
  struct single_node* next;
} single_node_t;

typedef struct {
  single_node_t* head;
  single_node_t* tail;
} single_linked_list_t;

single_node_t* single_linked_list_node_new(const single_node_data_t data);
void single_linked_list_node_free(single_node_t* const node, const single_node_data_free_t data_free);

void single_linked_list_new(single_linked_list_t* const ll);
bool single_linked_list_insert_head(single_linked_list_t* const ll, const single_node_data_t data);
bool single_linked_list_insert_tail(single_linked_list_t* const ll, const single_node_data_t data);
bool single_linked_list_pop_head(single_linked_list_t* const ll, single_node_data_t* const output);
bool single_linked_list_pop_tail(single_linked_list_t* const ll, single_node_data_t* const output);
void single_linked_list_remove_head(single_linked_list_t* const ll, const single_node_data_free_t data_free);
void single_linked_list_remove_tail(single_linked_list_t* const ll, const single_node_data_free_t data_free);
void single_linked_list_free(single_linked_list_t* const ll, const single_node_data_free_t data_free);

#endif