#ifndef REDUCT_CLOSURE_H
#define REDUCT_CLOSURE_H 1

struct reduct_item;

#include "defs.h"
#include "function.h"

/**
 * @brief Reduct closure
 * @defgroup closure Reduct closure
 * @file closure.h
 *
 * A closure is a function instance that has captured variables from its enclosing scope.
 *
 * @{
 */

#define REDUCT_CLOSURE_SMALL_MAX 4 ///< The maximum number of small constants.

/**
 * @brief Reduct closure structure.
 * @struct reduct_closure_t
 */
typedef struct reduct_closure
{
    reduct_function_t* function; ///< Pointer to the prototype function item.            ///< Pointer to upvalues array.
    reduct_handle_t* constants;  ///< The array of constant slots forming the constant template.
    reduct_handle_t smallConstants[REDUCT_CLOSURE_SMALL_MAX];
} reduct_closure_t;

/**
 * @brief Deinitialize a closure structure.
 *
 * @param closure The closure to deinitialize.
 */
REDUCT_API void reduct_closure_deinit(reduct_closure_t* closure);

/**
 * @brief Allocate a new closure.
 *
 * @param reduct The Reduct structure.
 * @param function The prototype function item.
 * @return A pointer to the newly allocated closure.
 */
REDUCT_API reduct_closure_t* reduct_closure_new(struct reduct* reduct, reduct_function_t* function);

/** @} */

#endif