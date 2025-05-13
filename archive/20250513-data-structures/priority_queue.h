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

void priority_queue_new(priority_queue_t* const priority_queue, const priority_queue_data_comparison_t compare);

inline bool priority_queue_is_empty(const priority_queue_t* const priority_queue) {
  return priority_queue->linked_list.head == NULL;
}

bool priority_queue_push(priority_queue_t* const priority_queue, const priority_queue_data_t data);
bool priority_queue_pop(priority_queue_t* const priority_queue, priority_queue_data_t* const output, const priority_queue_data_free_t data_free);
priority_queue_data_t priority_queue_peek(const priority_queue_t* const priority_queue);

void priority_queue_free(priority_queue_t* const priority_queue, const priority_queue_data_free_t data_free);

#endif