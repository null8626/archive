#include "binary_tree.h"
#include "linked_list.h"

#include <stdlib.h>

#if defined(_DEBUG) || !defined(NDEBUG)
#define _BINARY_TREE_ASSERT(expr)  \
  if (!(expr)) {                   \
    return 0;                      \
  }
#else
#define _BINARY_TREE_ASSERT(expr)
#endif

node_t* node_new(const node_data_t data) {
  node_t* const node = malloc(sizeof(node_t));

  if (node != NULL) {
    node->data = data;
    node->left = NULL;
    node->right = NULL;
  }
  
  return node;
}

static bool node_bst_add_ref(node_t** const node, const node_data_t data, const node_data_comparison_t compare) {
  node_t* const n = *node;

  return n == NULL ? (*node = node_new(data)) != NULL : node_bst_add(n, data, compare);
}

bool node_bst_add(node_t* const node, const node_data_t data, const node_data_comparison_t compare) {
  _BINARY_TREE_ASSERT(node != NULL && compare != NULL);

  const node_data_comparison_result_t comparison_result = compare(node->data, data);
  node_t** addition_node;

  if (comparison_result < 0) {
    addition_node = &node->right;
  } else if (comparison_result > 0) {
    addition_node = &node->left;
  } else {
    return true;
  }

  return node_bst_add_ref(addition_node, data, compare);
}

bool node_bst_traverse_breadth(node_t* const node, const node_traversal_callback_t callback) {
  _BINARY_TREE_ASSERT(node != NULL && callback != NULL);

  bool success = false;
  single_linked_list_t queue;

  single_linked_list_new(&queue);

  if (!single_linked_list_insert_tail(&queue, (const single_node_data_t)node)) {
    goto NODE_BST_TRAVERSE_BREADTH_END;
  }

  node_t* current;

  while (single_linked_list_pop_head(&queue, (single_node_data_t* const)&current, NULL)) {
    callback(current);

    if ((current->left != NULL && !single_linked_list_insert_tail(&queue, (const single_node_data_t)current->left)) || (current->right != NULL && !single_linked_list_insert_tail(&queue, (const single_node_data_t)current->right))) {
      goto NODE_BST_TRAVERSE_BREADTH_END;
    }
  }

  success = true;

NODE_BST_TRAVERSE_BREADTH_END:
  single_linked_list_free(&queue, NULL);

  return success;
}

void node_bst_traverse_depth(node_t* const node, const node_bst_traverse_order_t order, const node_traversal_callback_t callback) {
#if defined(_DEBUG) || !defined(NDEBUG)
  if (callback == NULL) {
    return;
  }
#endif

  if (node != NULL) {
    node_t* a, *b;

    if (order == NODE_BST_TRAVERSE_ORDER_ASCENDING) {
      a = node->left;
      b = node->right;
    } else if (order == NODE_BST_TRAVERSE_ORDER_DESCENDING) {
      a = node->right;
      b = node->left;
    } else {
      return;
    }

    node_bst_traverse_depth(a, order, callback);

    callback(node);
    
    node_bst_traverse_depth(b, order, callback);
  }
}

node_t* node_bst_min(node_t* node) {
  return node == NULL || node->left == NULL ? node : node_bst_min(node->left);
}

node_t** node_bst_min_ref(node_t** node) {  
  _BINARY_TREE_ASSERT(node != NULL);

  node_t* const n = *node;

  return n == NULL || n->left == NULL ? node : node_bst_min_ref(&n->left);
}

node_t* node_bst_max(node_t* node) {
  return node == NULL || node->right == NULL ? node : node_bst_max(node->right);
}

node_t** node_bst_max_ref(node_t** node) {
  _BINARY_TREE_ASSERT(node != NULL);

  node_t* const n = *node;

  return n == NULL || n->right == NULL ? node : node_bst_max_ref(&n->right);
}

static node_data_t node_bst_standard_data_copy(const node_data_t data) {
  return data;
}

bool node_bst_remove(node_t** node, const node_data_comparison_t compare, node_data_copy_t data_copy, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument) {
  _BINARY_TREE_ASSERT(node != NULL);

  if (data_copy == NULL) {
    data_copy = node_bst_standard_data_copy;
  }

  node_t* n = *node, *replacement = NULL;

  if (n != NULL) {
    if (n->left == NULL) {
      replacement = n->right;
    } else if (n->right == NULL) {
      replacement = n->left;
    } else {
      node_t* const min_right = node_bst_min(n->right);
  
      if (data_free != NULL && n->data != NULL) {
        data_free(n->data, data_free_argument);
      }
  
      if ((n->data = data_copy(min_right->data)) == NULL) {
        node_free(n, data_free, data_free_argument);
        *node = NULL;
  
        return false;
      }
      
      return node_bst_find_and_remove(&n->right, min_right->data, compare, data_copy, data_free, data_free_argument);
    }

    node_free(n, data_free, data_free_argument);
    *node = replacement;
  }

  return true;
}

bool node_bst_find_and_remove(node_t** node, const node_data_t data, const node_data_comparison_t compare, const node_data_copy_t data_copy, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument) {
  _BINARY_TREE_ASSERT(node != NULL && compare != NULL);
  
  node_t* n;

  while ((n = *node) != NULL) {
    const node_data_comparison_result_t comparison_result = compare(n->data, data);

    if (comparison_result < 0) {
      node = &n->right;
    } else if (comparison_result > 0) {
      node = &n->left;
    } else {
      return node_bst_remove(node, compare, data_copy, data_free, data_free_argument);
    }
  }

  return true;
}

node_t* node_bst_find(node_t* node, const node_data_t data, const node_data_comparison_t compare) {
  _BINARY_TREE_ASSERT(compare != NULL);

  while (node != NULL) {
    const node_data_comparison_result_t comparison_result = compare(node->data, data);

    if (comparison_result < 0) {
      node = node->right;
    } else if (comparison_result > 0) {
      node = node->left;
    } else {
      break;
    }
  }

  return node;
}

node_t** node_bst_find_ref(node_t** node, const node_data_t data, const node_data_comparison_t compare) {
  _BINARY_TREE_ASSERT(node != NULL && compare != NULL);

  node_t* n;

  while ((n = *node) != NULL) {
    const node_data_comparison_result_t comparison_result = compare(n->data, data);

    if (comparison_result < 0) {
      node = &n->right;
    } else if (comparison_result > 0) {
      node = &n->left;
    } else {
      break;
    }
  }

  return node;
}

node_t* node_bst_approx(node_t* node, const node_bst_approx_constraint_t constraint, const node_data_t data, const node_data_comparison_t compare) {
  _BINARY_TREE_ASSERT(node != NULL && compare != NULL && (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_CEIL || constraint == NODE_BST_APPROX_CONSTRAINT_FLOOR));
  
  node_t* result = NULL;

  while (node != NULL) {
    const node_data_comparison_result_t comparison_result = compare(node->data, data);

    if (comparison_result > 0) {
      if (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_CEIL) {
        result = node;
      }

      node = node->left;
    } else if (comparison_result < 0) {
      if (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_FLOOR) {
        result = node;
      }

      node = node->right;
    } else {
      return node;
    }
  }
  
  return result;
}

node_t** node_bst_approx_ref(node_t** node, const node_bst_approx_constraint_t constraint, const node_data_t data, const node_data_comparison_t compare) {
  _BINARY_TREE_ASSERT(node != NULL && compare != NULL && (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_CEIL || constraint == NODE_BST_APPROX_CONSTRAINT_FLOOR));
  
  node_t** result = NULL;
  node_t* n;

  while ((n = *node) != NULL) {
    const node_data_comparison_result_t comparison_result = compare(n->data, data);

    if (comparison_result > 0) {
      if (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_CEIL) {
        result = node;
      }

      node = &n->left;
    } else if (comparison_result < 0) {
      if (constraint == NODE_BST_APPROX_CONSTRAINT_CLOSEST || constraint == NODE_BST_APPROX_CONSTRAINT_FLOOR) {
        result = node;
      }

      node = &n->right;
    } else {
      return node;
    }
  }
  
  return result;
}

static size_t node_max(const size_t a, const size_t b) {
  return a > b ? a : b;
}

size_t node_height(const node_t* const node) {
  return node == NULL ? 0 : 1 + node_max(node_height(node->left), node_height(node->right));
}

size_t node_size(const node_t* const node) {
  return node == NULL ? 0 : 1 + node_size(node->left) + node_size(node->right);
}

void node_free(node_t* const node, const node_data_free_t data_free, const node_data_free_argument_t data_free_argument) {
  if (node != NULL) {
    if (data_free != NULL && node->data != NULL) {
      data_free(node->data, data_free_argument);
    }

    node_free(node->left, data_free, data_free_argument);
    node_free(node->right, data_free, data_free_argument);

    free(node);
  }
}