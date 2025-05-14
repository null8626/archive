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

/**
 * @brief Creates a single linked list node.
 * 
 * @param data The data to be carried in this node. Optional.
 * @see single_linked_list_node_free
 * @see single_linked_list_new
 * @return The new node. This can be NULL.
 */
single_node_t* single_linked_list_node_new(const single_node_data_t data);

/**
 * @brief Frees the single linked list node.
 * 
 * @param data The node to be freed.
 * @param data_free The node data's free function to be called upon disposal. Optional.
 * @see single_linked_list_free
 */
void single_linked_list_node_free(single_node_t* const node, const single_node_data_free_t data_free);

/**
 * @brief Initiates a new single linked list.
 * 
 * @param ll A pointer to the linked list to be initiated.
 * @see single_linked_list_insert_head
 * @see single_linked_list_insert_tail
 * @see single_linked_list_pop_head
 * @see single_linked_list_pop_tail
 * @see single_linked_list_free
 */
void single_linked_list_new(single_linked_list_t* const ll);

/**
 * @brief Inserts a new element to the head of the single linked list.
 * 
 * @param ll A pointer to the linked list to use.
 * @param data The new element's data.
 * @see single_linked_list_insert_tail
 * @see single_linked_list_pop_head
 * @see single_linked_list_pop_tail
 * @return Whether the operation was successful.
 */
bool single_linked_list_insert_head(single_linked_list_t* const ll, const single_node_data_t data);

/**
 * @brief Inserts a new element to the tail of the single linked list.
 * 
 * @param ll A pointer to the linked list to use.
 * @param data The new element's data.
 * @see single_linked_list_insert_head
 * @see single_linked_list_pop_head
 * @see single_linked_list_pop_tail
 * @return Whether the operation was successful.
 */
bool single_linked_list_insert_tail(single_linked_list_t* const ll, const single_node_data_t data);

/**
 * @brief Removes and returns an element from the head of the single linked list.
 * 
 * @param ll A pointer to the linked list to use.
 * @param output A pointer to the output value. Optional.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @see single_linked_list_pop_tail
 * @see single_linked_list_free
 * @return Whether the operation was successful.
 */
bool single_linked_list_pop_head(single_linked_list_t* const ll, single_node_data_t* const output, const single_node_data_free_t data_free);

/**
 * @brief Removes and returns an element from the tail of the single linked list.
 * 
 * @param ll A pointer to the linked list to use.
 * @param output A pointer to the output value. Optional.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @see single_linked_list_pop_head
 * @see single_linked_list_free
 * @return Whether the operation was successful.
 */
bool single_linked_list_pop_tail(single_linked_list_t* const ll, single_node_data_t* const output, const single_node_data_free_t data_free);

/**
 * @brief Frees every element before also freeing the single linked list itself.
 * 
 * @param ll A pointer to the linked list to be freed.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @see single_linked_list_node_free
 * @note After this, the linked list is empty, as if it's fresh back from single_linked_list_new again.
 */
void single_linked_list_free(single_linked_list_t* const ll, const single_node_data_free_t data_free);

#endif