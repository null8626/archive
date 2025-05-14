#ifndef _BINARY_TREE_H
#define _BINARY_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NODE_BST_TRAVERSE_ORDER_ASCENDING 0
#define NODE_BST_TRAVERSE_ORDER_DESCENDING 1

#define NODE_BST_APPROX_CONSTRAINT_CLOSEST 0
#define NODE_BST_APPROX_CONSTRAINT_FLOOR 1
#define NODE_BST_APPROX_CONSTRAINT_CEIL 2

typedef void* node_data_t;
typedef void* node_data_free_argument_t;
typedef uint8_t node_bst_traverse_order_t;
typedef uint8_t node_bst_approx_constraint_t;
typedef int64_t node_data_comparison_result_t;
typedef node_data_t (*node_data_copy_t)(node_data_t);
typedef void (*node_data_free_t)(node_data_t, node_data_free_argument_t);
typedef node_data_comparison_result_t (*node_data_comparison_t)(node_data_t, node_data_t);
typedef void (*node_traversal_callback_t)(const node_t* const);

typedef struct node {
  node_data_t data;
  struct node* left, *right;
} node_t;

/**
 * @brief Creates a brand new binary tree node.
 * 
 * @param data The data to be carried by this node. Optional.
 * @see node_bst_add
 * @see node_bst_traverse_breadth
 * @see node_bst_traverse_depth
 * @see node_bst_min
 * @see node_bst_min_ref
 * @see node_bst_max
 * @see node_bst_max_ref
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @see node_bst_find
 * @see node_bst_find_ref
 * @see node_bst_approx
 * @see node_bst_approx_ref
 * @see node_size
 * @see node_height
 * @see node_free
 * @return The created node. Can be NULL.
 */
node_t* node_new(const node_data_t data);

/**
 * @brief Adds a new node to the binary search tree.
 * 
 * @param node The node to start from.
 * @param data The data to be carried by this node.
 * @param compare The comparison function to be used.
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @return Whether the addition was successful.
 */
bool node_bst_add(node_t* const node, const node_data_t data, const node_data_comparison_t compare);

/**
 * @brief Traverses through the binary search tree using breadth-first search (BFS).
 * 
 * @param node The node to start from.
 * @param callback The callback function that receives each node.
 * @see node_bst_traverse_depth
 * @return Whether the traversal was successful.
 */
bool node_bst_traverse_breadth(node_t* const node, const node_traversal_callback_t callback);

/**
 * @brief Traverses through the binary search tree in a specified order. This is essentially a depth-first search (DFS).
 * 
 * @param node The node to start from.
 * @param order The specified order.
 * @param callback The callback function that receives each node.
 * @see node_bst_traverse_breadth
 */
void node_bst_traverse_depth(node_t* const node, const node_bst_traverse_order_t order, const node_traversal_callback_t callback);

/**
 * @brief Returns the node with the smallest value (leftmost) from this binary search tree.
 * 
 * @param node The node to start from.
 * @see node_bst_min_ref
 * @see node_bst_max
 * @see node_bst_max_ref
 * @return The node with the smallest value. (leftmost)
 */
node_t* node_bst_min(node_t* node);

/**
 * @brief Returns a pointer to the node with the smallest value (leftmost) from this binary search tree.
 * 
 * @param node A pointer to the node to start from.
 * @see node_bst_min
 * @see node_bst_max
 * @see node_bst_max_ref
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @see node_bst_find_ref
 * @see node_bst_approx_ref
 * @return A pointer to the node with the smallest value. (leftmost)
 */
node_t** node_bst_min_ref(node_t** node);

/**
 * @brief Returns the node with the largest value (rightmost) from this binary search tree.
 * 
 * @param node The node to start from.
 * @see node_bst_min
 * @see node_bst_min_ref
 * @see node_bst_max_ref
 * @return The node with the smallest value. (rightmost)
 */
node_t* node_bst_max(node_t* node);

/**
 * @brief Returns a pointer to the node with the largest value (rightmost) from this binary search tree.
 * 
 * @param node A pointer to the node to start from.
 * @see node_bst_min
 * @see node_bst_min_ref
 * @see node_bst_max
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @see node_bst_find_ref
 * @see node_bst_approx_ref
 * @see node_free
 * @return A pointer to the node with the largest value. (rightmost)
 */
node_t** node_bst_max_ref(node_t** node);

/**
 * @brief Removes a node.
 * 
 * @param node A pointer to the node to remove.
 * @param compare The comparison function to be used.
 * @param data_copy The node data's copy function to be called. Optional.
 * @param data_free The node data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 * @see node_bst_find_and_remove
 * @see node_free
 * @return Whether the removal was successful. Depends if data_copy fails. If this fails, it is FATAL and the binary search tree can NO LONGER be used.
 * @note Has no effect if the pointer pointed by node is NULL.
 */
bool node_bst_remove(node_t** node, const node_data_comparison_t compare, node_data_copy_t data_copy, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument);

/**
 * @brief Removes a node that matches the specified value.
 * 
 * @param node A pointer to the node to start from.
 * @param data The data to be searched.
 * @param compare The comparison function to be used.
 * @param data_copy The node data's copy function to be called. Optional.
 * @param data_free The node data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 * @see node_bst_remove
 * @see node_bst_find
 * @see node_bst_find_ref
 * @see node_bst_approx
 * @see node_bst_approx_ref
 * @see node_free
 * @return Whether the removal was successful. Depends if data_copy fails. If this fails, it is FATAL and the binary search tree can NO LONGER be used.
 * @note Has no effect if the pointer pointed by node is NULL.
 */
bool node_bst_find_and_remove(node_t** node, const node_data_t data, const node_data_comparison_t compare, const node_data_copy_t data_copy, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument);

/**
 * @brief Finds a node that matches the specified value.
 * 
 * @param node The node to start from.
 * @param data The data to be searched.
 * @param compare The comparison function to be used.
 * @see node_bst_find_ref
 * @see node_bst_find_and_remove
 * @see node_bst_approx
 * @see node_bst_approx_ref
 * @return The node that matches the value. Can be NULL.
 */
node_t* node_bst_find(node_t* node, const node_data_t data, const node_data_comparison_t compare);

/**
 * @brief Finds a pointer to the node that matches the specified value.
 * 
 * @param node A pointer to the node to start from.
 * @param data The data to be searched.
 * @param compare The comparison function to be used.
 * @see node_bst_find
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @see node_bst_min_ref
 * @see node_bst_max_ref
 * @see node_bst_approx
 * @see node_bst_approx_ref
 * @return A pointer to the node that matches the value. Can be NULL.
 */
node_t** node_bst_find_ref(node_t** node, const node_data_t data, const node_data_comparison_t compare);

/**
 * @brief Finds a node that is the closest to the specified value and under the specified constraint.
 * 
 * @param node The node to start from.
 * @param constraint The specified constraint. Whether to retrieve the closest, floor, or ceil node.
 * @param data The data to be searched.
 * @param compare The comparison function to be used.
 * @see node_bst_approx_ref
 * @see node_bst_find
 * @see node_bst_find_ref
 * @see node_bst_find_and_remove
 * @return The node that is the closest to the value. Can be NULL.
 */
node_t* node_bst_approx(node_t* node, const node_bst_approx_constraint_t constraint, const node_data_t data, const node_data_comparison_t compare);

/**
 * @brief Finds a pointer to the node that is the closest to the specified value and under the specified constraint.
 * 
 * @param node A pointer to the node to start from.
 * @param constraint The specified constraint. Whether to retrieve the closest, floor, or ceil node.
 * @param data The data to be searched.
 * @param compare The comparison function to be used.
 * @see node_bst_approx
 * @see node_bst_remove
 * @see node_bst_find_and_remove
 * @see node_bst_find
 * @see node_bst_find_ref
 * @see node_bst_min_ref
 * @see node_bst_max_ref
 * @return A pointer to the node that is the closest to the value. Can be NULL.
 */
node_t** node_bst_approx_ref(node_t** node, const node_bst_approx_constraint_t constraint, const node_data_t data, const node_data_comparison_t compare);

/**
 * @brief Retrieves the entire binary tree's height.
 * 
 * @param node The binary tree node to start from.
 * @see node_size
 * @return The binary tree height, including its root node.
 * @note Has no effect if the starting node is NULL.
 */
size_t node_height(const node_t* const node);

/**
 * @brief Retrieves the entire binary tree's size.
 * 
 * @param node The binary tree node to start from.
 * @see node_height
 * @return The binary tree size, including its root node.
 * @note Has no effect if the starting node is NULL.
 */
size_t node_size(const node_t* const node);

/**
 * @brief Deletes the entire binary tree node and its child nodes.
 * 
 * @param node The binary tree node to start from.
 * @param data_free The node data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 * @note Has no effect if the starting node is NULL.
 */
void node_free(node_t* const node, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument);

#endif