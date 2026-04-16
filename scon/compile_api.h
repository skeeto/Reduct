#ifndef SCON_COMPILE_API_H
#define SCON_COMPILE_API_H 1

#include "core_api.h"
#include "item_api.h"

/**
 * @brief Opaque compiled SCON function structure.
 */
typedef struct _scon_function scon_function_t;

/**
 * @brief Compiles a SCON AST into a callable bytecode function.
 *
 * @warning The jump buffer must have been set using `SCON_CATCH` before calling this function.
 * 
 * @param scon The SCON structure.
 * @param ast The root AST item to compile (usually a list of expressions).
 * @return A pointer to the compiled function. Will throw via SCON_LONGJMP on error.
 */
SCON_API scon_function_t* scon_compile(scon_t* scon, scon_item_t* ast);

#endif