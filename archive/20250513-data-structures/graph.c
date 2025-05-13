/* Posted on 10 April 2025 (original), 13 May 2025 (current) */

#include "graph.h"

#include <stdlib.h>

#ifdef GRAPH_DIJKSTRA
#include "priority_queue.h"

#include <string.h>
#include <float.h>

#define GRAPH_DIJKSTRA_MAX_DISTANCE DBL_MAX

typedef struct {
  vertex_t* vertex;
  vertex_dijkstra_distance_t distance;
} vertex_priority_queue_entry_t;

static void vertex_cleanup_with_argument(void* const data, void* const _unused) {
  if (data != NULL) {
    free(data);
  }
}
#endif

#ifdef GRAPH_TRAVERSAL
#include "linked_list.h"
#include "hash_table.h"

#include <string.h>
#endif

#if defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
#if defined(_DEBUG) || !defined(NDEBUG)
#define _GRAPH_ASSERT(expr)  \
  if (!(expr)) {             \
    return 0;                \
  }
#else
#define _GRAPH_ASSERT(expr)
#endif

static void vertex_simple_cleanup(void* const data) {
  if (data != NULL) {
    free(data);
  }
}
#endif

vertex_t* vertex_new(const vertex_data_t data) {
  vertex_t* const vertex = malloc(sizeof(vertex_t));

  if (vertex != NULL) {
    vertex->data = data;
    vertex->edges = NULL;
    vertex->edges_length = 0;
    vertex->edges_capacity = 0;
    vertex->freed = false;
  }

  return vertex;
}

/**
 * @brief Checks if two edges are basically the same thing.
 * 
 * @param edge1 The first edge to compare.
 * @param edge2 The second edge to compare.
 * @return Whether they are basically equal direction and purpose-wise.
 */
static bool vertex_edge_is_equal(const vertex_edge_t* const edge1, const vertex_edge_t* const edge2) {
  const bool is_bidirectional = edge1->direction == EDGE_DIRECTION_BIDIRECTIONAL && edge2->direction == EDGE_DIRECTION_BIDIRECTIONAL;

  if (edge1->a == edge2->a && edge1->b == edge2->b) {
    return is_bidirectional || (edge1->direction == EDGE_DIRECTION_A_TO_B && edge2->direction == EDGE_DIRECTION_A_TO_B) ||
                               (edge1->direction == EDGE_DIRECTION_B_TO_A && edge2->direction == EDGE_DIRECTION_B_TO_A);
  }

  else if (edge1->a == edge2->b && edge1->b == edge2->a) {
    return is_bidirectional || (edge1->direction == EDGE_DIRECTION_A_TO_B && edge2->direction == EDGE_DIRECTION_B_TO_A) ||
                               (edge1->direction == EDGE_DIRECTION_B_TO_A && edge2->direction == EDGE_DIRECTION_A_TO_B);
  }

  return false;
}

/**
 * @brief Tries to add an edge to the vertex's edges array.
 * 
 * @return Whether it works.
 * @note If it fails, the vertex's edges array length will be set to 0.
 */
static bool vertex_add_edge(vertex_t* const vertex, vertex_edge_t* const edge) {
  if (vertex->edges == NULL) {
    if ((vertex->edges = malloc(sizeof(vertex_edge_t*))) == NULL) {
      return false;
    }

    vertex->edges_capacity = 1;
  } else {
    for (size_t i = 0; i < vertex->edges_length; i++) {
      if (vertex_edge_is_equal(vertex->edges[i], edge)) {
        return true;
      }
    }

    if (vertex->edges_length == vertex->edges_capacity) {
      vertex->edges_capacity *= 2;
  
      vertex_edge_t** new_edges = realloc(vertex->edges, sizeof(vertex_edge_t*) * vertex->edges_capacity);
      
      if (new_edges == NULL) {
        return false;
      }
  
      vertex->edges = new_edges;
    }
  }

  vertex->edges[vertex->edges_length++] = edge;
  return true;
}

/**
 * @brief Disconnects a vertex from an edge by removing the edge from the vertex's edges array.
 * 
 * @param vertex The specified vertex.
 * @param edge The specified edge.
 * @note Has no effect if vertex is NULL or the vertex's edges array is empty.
 */
static void vertex_disconnect_from(vertex_t* const vertex, vertex_edge_t* const edge) {
  if (vertex == NULL || vertex->edges_length == 0) {
    return;
  }

  size_t i = 0;

  for (; i < vertex->edges_length; i++) {
    if (vertex->edges[i] == edge) {
      i++;
      break;
    }
  }

  for (; i < vertex->edges_length; i++) {
    vertex->edges[i - 1] = vertex->edges[i];
  }

  vertex->edges_length--;
  
  if (edge->a == vertex) {
    edge->a = NULL;
  }

  if (edge->b == vertex) {
    edge->b = NULL;
  }
}

/**
 * @brief Frees an edge and calls its disposal function if it exists.
 * 
 * @param edge The specified edge.
 * @param weight_free This edge's weight free function to be called upon disposal. Optional.
 */
static void vertex_free_edge(vertex_edge_t* const edge, const vertex_edge_weight_free_t weight_free) {
  if (weight_free != NULL) {
    weight_free(edge->weight);
  }
  
  free(edge);
}

vertex_edge_t* vertex_connect(vertex_t* const a, vertex_t* const b, const vertex_edge_weight_t weight, const vertex_edge_weight_free_t weight_free, const edge_direction_t direction) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  vertex_edge_t* const edge = malloc(sizeof(vertex_edge_t));

  if (edge != NULL) {
    edge->a = a;
    edge->b = b;
    edge->weight = weight;
    edge->direction = direction;
  
    if (!vertex_add_edge(a, edge)) {
      vertex_free_edge(edge, weight_free);
      
      return NULL;
    } else if (a != b && !vertex_add_edge(b, edge)) {
      vertex_disconnect_from(a, edge);
      vertex_free_edge(edge, weight_free);
  
      return NULL;
    }
  }

  return edge;
}

/**
 * @brief Tries to retrieve the other side of an edge.
 * 
 * @param vertex The specified vertex.
 * @param edge The specified edge.
 * @return The other side of the edge. Can be NULL if the edge is a loop.
 */
static vertex_t* vertex_get_other(const vertex_t* const vertex, const vertex_edge_t* const edge) {
  if (edge->a == vertex) {
    if (edge->b == vertex) {
      return NULL;
    } else {
      return edge->b;
    }
  } else {
    return edge->a;
  }
}

/**
 * @brief Disconnects both sides of an edge and deletes itself.
 * 
 * @param edge The specified edge.
 * @param weight_free This edge's weight free function to be called upon disposal. Optional.
 */
static void vertex_disconnect(vertex_edge_t* const edge, const vertex_edge_weight_free_t weight_free) {
  vertex_t* const a = edge->a;

  vertex_disconnect_from(a, edge);

  if (edge->b != a) {
    vertex_disconnect_from(edge->b, edge);
  }

  vertex_free_edge(edge, weight_free);
}

/**
 * @brief Checks if the source vertex can access a destination vertex using the specified edge.
 * 
 * @param source Source vertex.
 * @param edge The specified edge.
 * @param destination Reference to destination vertex. Required.
 * @return Whether the source vertex can access the destination vertex using the specified edge.
 */
static bool vertex_can_access(vertex_t* const source, vertex_edge_t* const edge, vertex_t** const destination) {
  if (edge->direction == EDGE_DIRECTION_BIDIRECTIONAL) {
    if ((*destination = vertex_get_other(source, edge)) == NULL) {
      *destination = source;
    }

    return true;
  }
  
  *destination = edge->a;
  
  if (edge->a == source) {
    if (edge->b == source) {
      return true;
    } else {
      *destination = edge->b;

      return edge->direction == EDGE_DIRECTION_A_TO_B;
    }
  } else {
    return edge->direction == EDGE_DIRECTION_B_TO_A;
  }
}

void vertex_free_all(vertex_t* const vertex, const vertex_data_free_t data_free, const vertex_edge_weight_free_t weight_free) {
  if (vertex == NULL || vertex->freed) {
    return;
  } else if (data_free != NULL) {
    data_free(vertex->data);
  }

  vertex->freed = true;

  do {
    vertex_edge_t* const edge = vertex->edges[0];
    vertex_t* const other = vertex_get_other(vertex, edge);

    vertex_disconnect(edge, weight_free);
    vertex_free_all(other, data_free, weight_free);
  } while (vertex->edges_length > 0);

  if (vertex->edges != NULL) {
    free(vertex->edges);
  }

  free(vertex);
}

#if defined(GRAPH_VERTEX_ITERATOR) || defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
void vertex_iterator_new(vertex_iterator_t* const it, vertex_t* const vertex) {
  it->index = 0;
  it->vertex = vertex;
}

vertex_edge_t* vertex_iterator_next(vertex_iterator_t* const it, vertex_t** const destination) {
  while (it->index < it->vertex->edges_length) {
    vertex_edge_t* const edge = it->vertex->edges[it->index];
    it->index++;

    if (vertex_can_access(it->vertex, edge, destination)) {
      return edge;
    }
  }

  *destination = NULL;
  return NULL;
}
#endif

#if defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
static size_t vertex_hash_function(const hash_table_key_t key, hash_function_additional_argument_t* const additional_argument) {
  return ((const vertex_hash_function_t)additional_argument->data)(((const vertex_t* const)key)->data, additional_argument->table_size);
}

static bool vertex_is_equal(const hash_table_key_t entry1, const hash_table_key_t entry2) {
  return (const vertex_t* const)entry1 == (const vertex_t* const)entry2;
}
#endif

#ifdef GRAPH_DIJKSTRA
static priority_queue_data_comparison_result_t vertex_priority_queue_entry_compare(const priority_queue_data_t data1, const priority_queue_data_t data2) {
  return ((const vertex_priority_queue_entry_t* const)data1)->distance - ((const vertex_priority_queue_entry_t* const)data2)->distance;
}

static priority_queue_data_t vertex_priority_queue_entry_copy(const priority_queue_data_t data) {
  vertex_priority_queue_entry_t* const new_entry = malloc(sizeof(vertex_priority_queue_entry_t));
  const vertex_priority_queue_entry_t* const old_entry = (const vertex_priority_queue_entry_t* const)data;

  if (new_entry != NULL) {
    new_entry->vertex = old_entry->vertex;
    new_entry->distance = old_entry->distance;
  }

  return (priority_queue_data_t)new_entry;
}

static inline hash_table_value_t vertex_distance_to_value(const vertex_dijkstra_distance_t distance) {
  return (hash_table_value_t)(*(size_t*)&distance);
}

bool vertex_dijkstra_new(vertex_dijkstra_t* const dijkstra, vertex_t* const start, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_dijkstra_distance_retrieval_t distance_retrieval) {
  _GRAPH_ASSERT(dijkstra != NULL && hash_table_size != 0 && start != NULL && hash_function != NULL && distance_retrieval != NULL);
  
  dijkstra->start = start;

  priority_queue_t priority_queue;
  hash_table_t distance_map;
  bool success = false;

  memset(&priority_queue, 0, sizeof(priority_queue_t));
  memset(&distance_map, 0, sizeof(hash_table_t));
  memset(&dijkstra->previous_map_entry_free, 0, sizeof(hash_table_entry_free_t));

  priority_queue_new(&priority_queue, vertex_priority_queue_entry_compare);

  dijkstra->previous_map_entry_free.value_free = vertex_cleanup_with_argument;

  if (!hash_table_new(&dijkstra->previous_map, hash_table_size, vertex_hash_function, (const hash_function_additional_argument_data_t)hash_function, vertex_is_equal, &dijkstra->previous_map_entry_free) ||
      !hash_table_new(&distance_map, hash_table_size, vertex_hash_function, (const hash_function_additional_argument_data_t)hash_function, vertex_is_equal, NULL)) {
    goto VERTEX_DIJKSTRA_NEW_END;
  }

  vertex_priority_queue_entry_t* const start_entry = malloc(sizeof(vertex_priority_queue_entry_t));

  if (start_entry == NULL) {
    goto VERTEX_DIJKSTRA_NEW_END;
  }

  start_entry->vertex = start;
  start_entry->distance = 0;

  if (!priority_queue_push(&priority_queue, (const priority_queue_data_t)start_entry)) {
    free(start_entry);

    goto VERTEX_DIJKSTRA_NEW_END;
  }
  
  vertex_iterator_t queue_vertex_iterator;

  while (!priority_queue_is_empty(&priority_queue)) {
    vertex_priority_queue_entry_t* queue_entry;
    
    if (!priority_queue_pop(&priority_queue, (priority_queue_data_t* const)&queue_entry, vertex_simple_cleanup)) {
      goto VERTEX_DIJKSTRA_NEW_END;
    }

    vertex_iterator_new(&queue_vertex_iterator, queue_entry->vertex);

    vertex_t* destination_vertex;
    vertex_edge_t* queue_vertex_edge;

    while ((queue_vertex_edge = vertex_iterator_next(&queue_vertex_iterator, &destination_vertex)) != NULL) {
      const vertex_dijkstra_distance_t new_distance = queue_entry->distance + distance_retrieval(queue_vertex_edge->weight);
      vertex_dijkstra_distance_t current_distance;

      if (!hash_table_get(&distance_map, (const hash_table_key_t)destination_vertex, (hash_table_value_t* const)&current_distance)) {
        current_distance = GRAPH_DIJKSTRA_MAX_DISTANCE;
      }

      if (new_distance < current_distance) {
        if (!hash_table_set(&distance_map, (const hash_table_key_t)destination_vertex, vertex_distance_to_value(new_distance))) {
          free(queue_entry);

          goto VERTEX_DIJKSTRA_NEW_END;
        }

        vertex_edge_entry_t* const previous_map_entry = malloc(sizeof(vertex_edge_entry_t));

        if (previous_map_entry == NULL) {
          free(queue_entry);

          goto VERTEX_DIJKSTRA_NEW_END;
        }

        previous_map_entry->vertex = queue_entry->vertex;
        previous_map_entry->edge = queue_vertex_edge;

        if (!hash_table_set(&dijkstra->previous_map, (const hash_table_key_t)destination_vertex, (const hash_table_value_t)previous_map_entry)) {
          free(previous_map_entry);
          free(queue_entry);

          goto VERTEX_DIJKSTRA_NEW_END;
        }

        vertex_priority_queue_entry_t* const new_queue_entry = malloc(sizeof(vertex_priority_queue_entry_t));

        if (new_queue_entry == NULL) {
          free(queue_entry);

          goto VERTEX_DIJKSTRA_NEW_END;
        }

        new_queue_entry->vertex = destination_vertex;
        new_queue_entry->distance = new_distance;

        if (!priority_queue_push(&priority_queue, (const priority_queue_data_t)new_queue_entry)) {
          free(new_queue_entry);
          free(queue_entry);

          goto VERTEX_DIJKSTRA_NEW_END;
        }
      }
    }

    free(queue_entry);
  }

  success = true;

VERTEX_DIJKSTRA_NEW_END:
  priority_queue_free(&priority_queue, vertex_simple_cleanup);
  hash_table_free(&distance_map);

  return success;
}

vertex_t* vertex_dijkstra_next(const vertex_dijkstra_t* const dijkstra, const vertex_t* const vertex, vertex_edge_t** const output_edge) {
  _GRAPH_ASSERT(dijkstra != NULL && vertex != NULL);

  vertex_t* output = NULL;

  if (vertex != dijkstra->start) {
    vertex_edge_entry_t* previous_map_entry;

    if (hash_table_get(&dijkstra->previous_map, (const hash_table_key_t)vertex, (hash_table_value_t* const)&previous_map_entry)) {
      output = previous_map_entry->vertex;

      if (output_edge != NULL) {
        *output_edge = previous_map_entry->edge;
      }
    }
  }

  return output;
}

void vertex_dijkstra_free(vertex_dijkstra_t* const dijkstra) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (dijkstra == NULL) {
    return;
  }
#endif
  
  hash_table_free((hash_table_t* const)&dijkstra->previous_map);
}
#endif

#ifdef GRAPH_TRAVERSAL
bool vertex_traverse_breadth(const vertex_t* const vertex, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_traversal_callback_t callback) {
  _GRAPH_ASSERT(vertex != NULL && hash_table_size != 0 && hash_function != NULL && callback != NULL);
  
  single_linked_list_t queue;
  
  single_linked_list_new(&queue);

  vertex_edge_entry_t* output_entry = NULL;
  bool success = false;
  hash_table_t visited;

  memset(&visited, 0, sizeof(hash_table_t));

  if (!hash_table_new(&visited, hash_table_size, vertex_hash_function, (const hash_function_additional_argument_data_t)hash_function, vertex_is_equal, NULL)) {
    goto VERTEX_TRAVERSE_BREADTH_END;
  }
  
  while (single_linked_list_pop_head(&queue, (single_node_data_t* const)&output_entry)) {
    if (hash_table_has(&visited, (const hash_table_key_t)output_entry->vertex)) {
      continue;
    }

    callback(output_entry);

    vertex_iterator_t iterator;
    vertex_t* destination;
    vertex_edge_t* destination_edge;
  
    vertex_iterator_new(&iterator, output_entry->vertex);
  
    while ((destination_edge = vertex_iterator_next(&iterator, &destination)) != NULL) {
      vertex_edge_entry_t* const entry = malloc(sizeof(vertex_edge_entry_t));
  
      if (entry == NULL) {
        goto VERTEX_TRAVERSE_BREADTH_END;
      }
  
      entry->vertex = destination;
      entry->edge = destination_edge;
  
      if (!single_linked_list_insert_tail(&queue, (const single_node_data_t)entry)) {
        free(entry);
  
        goto VERTEX_TRAVERSE_BREADTH_END;
      }
    }

    free(output_entry);
    output_entry = NULL;
  }

  success = true;

VERTEX_TRAVERSE_BREADTH_END:
  if (output_entry != NULL) {
    free(output_entry);
  }

  hash_table_free(&visited);
  single_linked_list_free(&queue, vertex_simple_cleanup);

  return success;
}

static bool vertex_traverse_depth_inner(hash_table_t* const visited, const vertex_t* const vertex, const vertex_traversal_callback_t callback) {
  if (!hash_table_set(visited, (const hash_table_key_t)vertex, (const hash_table_value_t)true)) {
    return false;
  }

  vertex_iterator_t iterator;
  vertex_edge_entry_t entry;

  vertex_iterator_new(&iterator, vertex);

  while ((entry.edge = vertex_iterator_next(&iterator, &entry.vertex)) != NULL) {
    if (hash_table_has(visited, (const hash_table_key_t)entry.vertex)) {
      continue;
    }

    callback(&entry);

    if (!vertex_traverse_depth_inner(visited, entry.vertex, callback)) {
      return false;
    }
  }

  return true;
}

bool vertex_traverse_depth(const vertex_t* const vertex, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_traversal_callback_t callback) {
  _GRAPH_ASSERT(vertex != NULL && hash_table_size != 0 && hash_function != NULL && callback != NULL);

  bool success = false;
  hash_table_t visited;

  memset(&visited, 0, sizeof(hash_table_t));

  if (!hash_table_new(&visited, hash_table_size, vertex_hash_function, (const hash_function_additional_argument_data_t)hash_function, vertex_is_equal, NULL)) {
    goto VERTEX_TRAVERSE_DEPTH_END;
  }

  success = vertex_traverse_depth_inner(&visited, vertex, callback);

VERTEX_TRAVERSE_DEPTH_END:
  hash_table_free(&visited);

  return success;
}
#endif
