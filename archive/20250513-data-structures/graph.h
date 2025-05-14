#ifndef _GRAPH_H
#define _GRAPH_H

#if defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
#include "hash_table.h"
#endif

#include <stdbool.h>
#include <stdint.h>

#define EDGE_DIRECTION_BIDIRECTIONAL 0
#define EDGE_DIRECTION_A_TO_B 1
#define EDGE_DIRECTION_B_TO_A 2

typedef uint8_t edge_direction_t;

typedef void* vertex_edge_weight_t;
typedef void (*vertex_edge_weight_free_t)(vertex_edge_weight_t);

typedef struct {
  void* a, *b;
  vertex_edge_weight_t weight;
  edge_direction_t direction;
} vertex_edge_t;

typedef void* vertex_data_t;
typedef void (*vertex_data_free_t)(vertex_data_t);

typedef struct {
  vertex_data_t data;
  vertex_edge_t** edges;
  size_t edges_length, edges_capacity;
  bool freed;
} vertex_t;

#if defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
typedef size_t (*vertex_hash_function_t)(const vertex_data_t, const size_t);

typedef struct {
  vertex_t* vertex;
  vertex_edge_t* edge;
} vertex_edge_entry_t;
#endif

#ifdef GRAPH_DIJKSTRA
typedef struct {
  hash_table_entry_free_t previous_map_entry_free;
  hash_table_t previous_map;
  vertex_t* start;
} vertex_dijkstra_t;

typedef double vertex_dijkstra_distance_t;
typedef vertex_dijkstra_distance_t (*vertex_dijkstra_distance_retrieval_t)(const vertex_edge_weight_t);
#endif

#if defined(GRAPH_VERTEX_ITERATOR) || defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
typedef struct {
  size_t index;
  vertex_t* vertex;
} vertex_iterator_t;
#endif

#ifdef GRAPH_TRAVERSAL
typedef void (*vertex_traversal_callback_t)(vertex_edge_entry_t* const);
#endif

/**
 * @brief Creates a brand new vertex not connected to anything.
 * 
 * @param data The data to be carried by this vertex. Optional.
 * @see vertex_connect
 * @see vertex_dijkstra_new
 * @see vertex_dijkstra_next
 * @see vertex_traverse_breadth
 * @see vertex_traverse_depth
 * @see vertex_free_all
 * @return The created vertex. Can be NULL.
 */
vertex_t* vertex_new(const vertex_data_t data);

/**
 * @brief Tries to connect two vertices by creating a brand new edge between them.
 * 
 * @param a The first vertex. Must NOT be NULL.
 * @param b The second vertex. Must NOT be NULL.
 * @param weight The data to be carried by this edge. Optional.
 * @param weight_free This edge's weight free function to be called upon disposal. Optional.
 * @param direction The direction of this edge.
 * @return The created edge. Can be NULL.
 */
vertex_edge_t* vertex_connect(vertex_t* const a, vertex_t* const b, const vertex_edge_weight_t weight, const vertex_edge_weight_free_t weight_free, const edge_direction_t direction);

/**
 * @brief Deletes the entire graph system.
 * 
 * @param vertex A single vertex to start from.
 * @param data_free The vertex data's free function to be called upon disposal. Optional.
 * @param weight_free This edge's weight free function to be called upon disposal. Optional.
 * @note Has no effect if vertex is NULL.
 */
void vertex_free_all(vertex_t* const vertex, const vertex_data_free_t data_free, const vertex_edge_weight_free_t weight_free);

#if defined(GRAPH_VERTEX_ITERATOR) || defined(GRAPH_DIJKSTRA) || defined(GRAPH_TRAVERSAL)
/**
 * @brief Initiates (or re-uses) a vertex iterator instance.
 * 
 * @param it The vertex iterator to initiate or re-use.
 * @param vertex The vertex instance to use.
 * @see vertex_iterator_next
 */
void vertex_iterator_new(vertex_iterator_t* const it, vertex_t* const vertex);

/**
 * @brief Tries to retrieve the next qualifying vertex if available.
 * 
 * @param it The vertex iterator to initiate or re-use.
 * @param destination The resulting vertex. Its pointer can be overwritten to NULL.
 * @return The edge instance connecting to the destination. Can be NULL.
 */
vertex_edge_t* vertex_iterator_next(vertex_iterator_t* const it, vertex_t** const destination);
#endif

#ifdef GRAPH_DIJKSTRA
/**
 * @brief Creates a new dijkstra instance from a starting vertex.
 * 
 * @param dijkstra The dijkstra instance to inititialize.
 * @param start The starting vertex.
 * @param hash_table_size Maximum capacity for the hash tables used.
 * @param hash_function The hash function to be used by the hash tables.
 * @param distance_retrieval The function to be used to retrieve distance numbers from edge weights.
 * @see vertex_dijkstra_next
 * @see vertex_dijkstra_free
 * @return Whether the creation was successful.
 */
bool vertex_dijkstra_new(vertex_dijkstra_t* const dijkstra, vertex_t* const start, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_dijkstra_distance_retrieval_t distance_retrieval);

/**
 * @brief Returns the next shortest distance vertex from the specified vertex.
 * 
 * @param dijkstra The dijkstra instance to be used.
 * @param start The vertex reference to use.
 * @param output_edge The edge connecting the result. Optional.
 * @see vertex_dijkstra_free
 * @return The next vertex. This and output_edge can be NULL.
 */
vertex_t* vertex_dijkstra_next(const vertex_dijkstra_t* const dijkstra, const vertex_t* const vertex, vertex_edge_t** const output_edge);

/**
 * @brief Frees the dijkstra instance.
 * 
 * @param dijkstra The dijkstra instance to be used.
 */
void vertex_dijkstra_free(vertex_dijkstra_t* const dijkstra);
#endif

#ifdef GRAPH_TRAVERSAL
/**
 * @brief Performs a breadth-first search (BFS) on the entire graph system.
 * 
 * @param vertex The starting vertex.
 * @param hash_table_size Maximum capacity for the hash tables used.
 * @param hash_function The hash function to be used by the hash tables.
 * @param callback The callback function for receiving traversal outputs.
 * @see vertex_traverse_depth
 * @return Whether the iteration was successful.
 */
bool vertex_traverse_breadth(const vertex_t* const vertex, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_traversal_callback_t callback);

/**
 * @brief Performs a depth-first search (DFS) on the entire graph system.
 * 
 * @param vertex The starting vertex.
 * @param hash_table_size Maximum capacity for the hash tables used.
 * @param hash_function The hash function to be used by the hash tables.
 * @param callback The callback function for receiving traversal outputs.
 * @see vertex_traverse_breadth
 * @return Whether the traversal was successful.
 */
bool vertex_traverse_depth(vertex_t* const vertex, const size_t hash_table_size, const vertex_hash_function_t hash_function, const vertex_traversal_callback_t callback);
#endif
#endif