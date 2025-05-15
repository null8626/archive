#ifndef _STACK_H
#define _STACK_H

#include <stdbool.h>
#include <stddef.h>

typedef void* stack_data_t;
typedef void* stack_data_free_argument_t;
typedef void (*stack_data_free_t)(stack_data_t, stack_data_free_argument_t);

typedef struct {
  stack_data_t* stack;
  size_t length;
  size_t capacity;
} stack_t;

/**
 * @brief Creates a brand new stack with an initial capacity.
 * 
 * @param stack The stack to create.
 * @param capacity The initial capacity. Cannot be zero.
 * @see stack_new
 * @see stack_push
 * @see stack_pop
 * @see stack_peek
 * @see stack_clear
 * @see stack_is_empty
 * @see stack_free
 * @return Whether the creation was successful.
 */
bool stack_new_with_capacity(stack_t* const stack, const size_t capacity);

/**
 * @brief Creates a brand new stack.
 * 
 * @param stack The stack to create.
 * @see stack_new_with_capacity
 * @see stack_push
 * @see stack_pop
 * @see stack_peek
 * @see stack_clear
 * @see stack_is_empty
 * @see stack_free
 * @return Whether the creation was successful.
 */
inline bool stack_new(stack_t* const stack) {
  return stack_new_with_capacity(stack, 2);
}

/**
 * @brief Inserts a new element to the top of the stack.
 * 
 * @param stack The stack to use.
 * @param data The new element's data.
 * @see stack_pop
 * @see stack_peek
 * @see stack_clear
 * @see stack_is_empty
 * @return Whether the operation was successful.
 */
bool stack_push(stack_t* const stack, const stack_data_t data);

/**
 * @brief Removes and returns an element from the top of the stack.
 * 
 * @param stack The stack to use.
 * @param output A pointer to the output value. Optional.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 * @see stack_peek
 * @see stack_clear
 * @see stack_is_empty
 * @see stack_free
 * @return Whether the operation was successful.
 */
bool stack_pop(stack_t* const stack, stack_data_t* const output, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument);

/**
 * @brief Peeks the next element to be returned by stack_pop without removing it.
 * 
 * @param stack The stack to use.
 * @see stack_pop
 * @see stack_clear
 * @see stack_is_empty
 * @see stack_free
 * @return The element at the top of the stack. This can be NULL.
 */
stack_data_t stack_peek(const stack_t* const stack);

/**
 * @brief Clears the entire stack.
 * 
 * @param stack The stack to use.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 * @see stack_pop
 * @see stack_is_empty
 * @see stack_free
 */
void stack_clear(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument);

/**
 * @brief Checks if the stack is empty.
 * 
 * @param stack The stack to check.
 * @see stack_pop
 * @see stack_peek
 * @see stack_free
 * @return Whether the stack is empty.
 */
inline bool stack_is_empty(const stack_t* const stack) {
  return stack->length == 0;
}

/**
 * @brief Frees every element before also freeing the stack itself.
 * 
 * @param stack The stack to be freed.
 * @param data_free The element data's free function to be called upon disposal. Optional.
 * @param data_free_argument The argument to be passed onto data_free upon called. Optional.
 */
void stack_free(stack_t* const stack, const stack_data_free_t data_free, const stack_data_free_argument_t data_free_argument);

#endif