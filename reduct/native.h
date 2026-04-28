#ifndef REDUCT_NATIVE_H
#define REDUCT_NATIVE_H 1

#include "defs.h"

struct reduct;

/**
 * @file native.h
 * @brief Native function registration.
 * @defgroup native Native Functions
 *
 * A "native" is a C function that can be called at runtime.
 *
 * @{
 */

/**
 * @brief Native function pointer type.
 *
 * @param reduct The Reduct structure.
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 * @return The result of the function.
 */
typedef reduct_handle_t (*reduct_native_fn)(struct reduct* reduct, reduct_size_t argc, reduct_handle_t* argv);

/**
 * @brief Native function definition structure.
 */
typedef struct
{
    const char* name;
    reduct_native_fn fn;
} reduct_native_t;

/**
 * @brief Register native functions.
 *
 * @param reduct The Reduct structure.
 * @param array An array of native function definitions.
 * @param count The number of functions in the array.
 */
REDUCT_API void reduct_native_register(struct reduct* reduct, reduct_native_t* array, reduct_size_t count);

/** @} */

#endif
