#include "priority_queue.h"

#include <stdlib.h>

#if defined(_DEBUG) || !defined(NDEBUG)
#define _PRIORITY_QUEUE_ASSERT(expr)  \
  if (!(expr)) {                      \
    return 0;                         \
  }
#else
#define _PRIORITY_QUEUE_ASSERT(expr)
#endif

void priority_queue_new(priority_queue_t* const priority_queue, const priority_queue_data_comparison_t compare) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (priority_queue == NULL || compare == NULL) {
    return;
  }
#endif

  single_linked_list_new(&priority_queue->linked_list);
  priority_queue->compare = compare;
}

bool priority_queue_push(priority_queue_t* const priority_queue, const priority_queue_data_t data) {
  _PRIORITY_QUEUE_ASSERT(priority_queue != NULL);

  single_node_t* node = single_linked_list_node_new((const single_node_data_t)data);

  if (node == NULL) {
    return false;
  }

  if (priority_queue->linked_list.head == NULL || priority_queue->compare(data, priority_queue->linked_list.head->data) < 0) {
    node->next = priority_queue->linked_list.head;
    
    if (priority_queue->linked_list.tail == priority_queue->linked_list.head) {
      priority_queue->linked_list.tail = node;
    }
    
    priority_queue->linked_list.head = node;
  } else {
    single_node_t* current = priority_queue->linked_list.head;

    while (current->next != NULL && priority_queue->compare(current->next->data, data) <= 0) {
      current = current->next;
    }

    if (priority_queue->linked_list.tail == current) {
      priority_queue->linked_list.tail = node;
    }

    node->next = current->next;
    current->next = node;
  }

  return true;
}

bool priority_queue_pop(priority_queue_t* const priority_queue, priority_queue_data_t* const output, const priority_queue_data_free_t data_free) {
  _PRIORITY_QUEUE_ASSERT(priority_queue != NULL);

  if (output == NULL) {
    single_linked_list_remove_head(&priority_queue->linked_list, data_free);
  } else {
    return single_linked_list_pop_head(&priority_queue->linked_list, (single_node_data_t* const)output);
  }

  return true;
}

priority_queue_data_t priority_queue_peek(const priority_queue_t* const priority_queue) {
  _PRIORITY_QUEUE_ASSERT(priority_queue != NULL);
  
  return priority_queue->linked_list.head == NULL ? NULL : priority_queue->linked_list.head->data;
}

void priority_queue_free(priority_queue_t* const priority_queue, const priority_queue_data_free_t data_free) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (priority_queue == NULL) {
    return;
  }
#endif
  
  single_linked_list_free(&priority_queue->linked_list, data_free);
}