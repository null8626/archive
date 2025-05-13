#include "linked_list.h"

#if defined(_DEBUG) || !defined(NDEBUG)
#define _LINKED_LIST_ASSERT(expr)  \
  if (!(expr)) {                   \
    return 0;                      \
  }
#else
#define _LINKED_LIST_ASSERT(expr)
#endif

void single_linked_list_new(single_linked_list_t* const ll) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (ll == NULL) {
    return;
  }
#endif

  ll->head = NULL;
  ll->tail = NULL;
}

single_node_t* single_linked_list_node_new(const single_node_data_t data) {
  single_node_t* node = malloc(sizeof(single_node_t));
  
  if (node != NULL) {
    node->data = data;
    node->next = NULL;
  }

  return node;
}

void single_linked_list_node_free(single_node_t* const node, const single_node_data_free_t data_free) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (node == NULL) {
    return;
  }
#endif

  if (data_free != NULL) {
    data_free(node->data);
  }

  free(node);
}

bool single_linked_list_insert_head(single_linked_list_t* const ll, const single_node_data_t data) {
  _LINKED_LIST_ASSERT(ll != NULL);

  single_node_t* node = single_linked_list_node_new(data);

  if (node != NULL) {
    node->next = ll->head;
    ll->head = node;

    if (ll->tail == NULL) {
      ll->tail = node;
    }

    return true;
  }

  return false;
}

bool single_linked_list_insert_tail(single_linked_list_t* const ll, const single_node_data_t data) {
  _LINKED_LIST_ASSERT(ll != NULL);
  
  single_node_t* node = single_linked_list_node_new(data);

  if (node != NULL) {
    if (ll->tail == NULL) {
      ll->head = node;
    } else {
      ll->tail->next = node;
    }

    ll->tail = node;

    return true;
  }

  return false;
}

bool single_linked_list_pop_head(single_linked_list_t* const ll, single_node_data_t* const output) {
  _LINKED_LIST_ASSERT(ll != NULL);

  if (ll->head != NULL) {
    if (output != NULL) {
      *output = ll->head->data;
    }

    single_node_t* const old_head = ll->head;
    
    if ((ll->head = ll->head->next) == NULL) {
      ll->tail = NULL;
    }

    free(old_head);

    return true;
  }

  return false;
}

bool single_linked_list_pop_tail(single_linked_list_t* const ll, single_node_data_t* const output) {
  _LINKED_LIST_ASSERT(ll != NULL);

  if (ll->head != NULL) {
    single_node_t* previous = NULL, *current = ll->head;
  
    for (current = ll->head; current->next != NULL; previous = current, current = current->next) {}
  
    if (output != NULL) {
      *output = current->data;
    }

    free(current);

    if (previous != NULL) {
      previous->next = NULL;
    } else {
      ll->head = NULL;
    }
  
    ll->tail = previous;
    
    return true;
  }

  return false;
}

void single_linked_list_remove_head(single_linked_list_t* const ll, const single_node_data_free_t data_free) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (ll == NULL) {
    return;
  }
#endif

  if (ll->head != NULL) {
    single_linked_list_node_free(ll->head, data_free);

    if ((ll->head = ll->head->next) == NULL) {
      ll->tail = NULL;
    }
  }
}

void single_linked_list_remove_tail(single_linked_list_t* const ll, const single_node_data_free_t data_free) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (ll == NULL) {
    return;
  }
#endif

  if (ll->head != NULL) {
    single_node_t* previous = NULL, *current = ll->head;

    for (current = ll->head; current->next != NULL; previous = current, current = current->next) {}

    single_linked_list_node_free(current, data_free);

    if (previous != NULL) {
      previous->next = NULL;
    } else {
      ll->head = NULL;
    }

    ll->tail = previous;
  }
}

void single_linked_list_free(single_linked_list_t* const ll, const single_node_data_free_t data_free) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (ll == NULL) {
    return;
  }
#endif

  single_node_t* node = ll->head;

  while (node != NULL) {
    single_linked_list_node_free(node, data_free);

    node = node->next;
  }

  ll->head = NULL;
  ll->tail = NULL;
}