#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H

#include "linked_list.h"
#include <stdint.h>

typedef single_node_data_t priority_queue_data_t;
typedef void (*priority_queue_data_free_t)(priority_queue_data_t);
typedef int64_t priority_queue_data_comparison_result_t;
typedef priority_queue_data_comparison_result_t (*priority_queue_data_comparison_t)(const priority_queue_data_t, const priority_queue_data_t);

typedef struct {
  single_linked_list_t linked_list;
  priority_queue_data_comparison_t compare;
} priority_queue_t;

/**
 * @brief Creates a brand new priority queue.
 * 
 * @param priority_queue The priority queue to create.
 * @param compare The comparison function to help sort each element's priority.
 * @see priority_queue_push
 * @see priority_queue_peek
 * @see priority_queue_pop
 * @see priority_queue_is_empty
 * @see priority_queue_free
 */
void priority_queue_new(priority_queue_t* const priority_queue, const priority_queue_data_comparison_t compare);

/**
 * @brief Checks if the priority queue is empty.
 * 
 * @param priority_queue The priority queue to check.
 * @see priority_queue_peek
 * @see priority_queue_pop
 * @see priority_queue_free
 * @return Whether the priority queue is empty.
 */
inline bool priority_queue_is_empty(const priority_queue_t* const priority_queue) {
  return priority_queue->linked_list.head == NULL;
}

/**
 * @brief Inserts a new element to the priority queue.
 * 
 * @param priority_queue The priority queue to use.
 * @param data The new element's data.
 * @see priority_queue_peek
 * @see priority_queue_pop
 * @return Whether the operation was successful.
 */
bool priority_queue_push(priority_queue_t* const priority_queue, const priority_queue_data_t data);

/**
 * @brief Removes and returns an element from the priority queue based on highest priority.
 * 
 * @param priority_queue The priority queue to use.
 * @param output A pointer to the output value. Optional.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @see priority_queue_peek
 * @see priority_queue_free
 * @return Whether the operation was successful.
 */
bool priority_queue_pop(priority_queue_t* const priority_queue, priority_queue_data_t* const output, const priority_queue_data_free_t data_free);

/**
 * @brief Peeks the next element to be returned by priority_queue_pop without removing it.
 * 
 * @param priority_queue The priority queue to use.
 * @see priority_queue_pop
 * @see priority_queue_free
 * @return The next element's data based on highest priority. This can be NULL.
 */
priority_queue_data_t priority_queue_peek(const priority_queue_t* const priority_queue);

/**
 * @brief Frees every element before also freeing the priority queue itself.
 * 
 * @param priority_queue The priority queue to be freed.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 */
void priority_queue_free(priority_queue_t* const priority_queue, const priority_queue_data_free_t data_free);

#endif