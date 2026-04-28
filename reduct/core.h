#ifndef REDUCT_CORE_H
#define REDUCT_CORE_H 1

struct reduct_item;

#include "atom.h"
#include "error.h"
#include "item.h"
#include "list.h"

/**
 * @file core.h
 * @brief Core definitions and structures.
 * @defgroup core
 *
 * @{
 */

#define REDUCT_BUCKETS_MAX 128 ///< Amount of buckets used for intering atoms.
#define REDUCT_CONSTANTS_MAX 8 ///< Maximum amount of predefined constants.

#define REDUCT_GC_THRESHOLD_INITIAL 128 ///< Initial blocks allocated threshold for garbage collection.

/**
 * @brief Input flags.
 */
typedef enum
{
    REDUCT_INPUT_FLAG_NONE = 0,
    REDUCT_INPUT_FLAG_OWNED = 1 ///< The input buffer is owned by the input structure and should be freed.
} reduct_input_flags_t;

/**
 * @brief Input structure.
 * @struct reduct_input_t
 */
typedef struct reduct_input
{
    struct reduct_input* prev;
    const char* buffer;
    const char* end;
    reduct_input_flags_t flags;
    char path[REDUCT_PATH_MAX];
} reduct_input_t;

/**
 * @brief Constant structure.
 * @struct reduct_constant_t
 */
typedef struct reduct_constant
{
    struct reduct_atom* name;
    struct reduct_item* item;
} reduct_constant_t;

/**
 * @brief State structure.
 * @struct reduct_t
 */
typedef struct reduct
{
    reduct_size_t blocksAllocated;
    reduct_size_t gcThreshold;
    reduct_item_block_t* block;
    struct reduct_item* freeList;
    reduct_input_t* input;
    reduct_jmp_buf_t jmp;
    reduct_item_block_t firstBlock;
    reduct_input_t firstInput;
    reduct_item_t* trueItem;
    reduct_item_t* falseItem;
    reduct_item_t* nilItem;
    reduct_item_t* piItem;
    reduct_item_t* eItem;
    struct reduct_atom* atomBuckets[REDUCT_BUCKETS_MAX];
    reduct_constant_t constants[REDUCT_CONSTANTS_MAX];
    reduct_uint32_t constantCount;
    reduct_error_t* error;
    struct reduct_eval_state* evalState;
    int argc;
    char** argv;
} reduct_t;

/**
 * @brief Create a new Reduct structure.
 *
 * @param error Pointer to the error structure to be used for error reporting.
 * @return A pointer to the newly allocated Reduct structure.
 */
REDUCT_API reduct_t* reduct_new(reduct_error_t* error);

/**
 * @brief Free the Reduct structure.
 *
 * @param reduct Pointer to the Reduct structure to free.
 */
REDUCT_API void reduct_free(reduct_t* reduct);

/**
 * @brief Set the command line arguments for the Reduct structure.
 *
 * Will be utilized by the `(args!)` native.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param argc The number of arguments.
 * @param argv The argument strings.
 */
REDUCT_API void reduct_args_set(reduct_t* reduct, int argc, char** argv);

/**
 * @brief Register a constant in a Reduct structure.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param name The name of the constant.
 * @param item The item associated with the constant.
 */
REDUCT_API void reduct_constant_register(reduct_t* reduct, const char* name, struct reduct_item* item);

/**
 * @brief Create a new input structure and push it onto the input stack.
 *
 * @param reduct The Reduct structure.
 * @param buffer The input buffer.
 * @param length The length of the input buffer.
 * @param path The path to the input file.
 * @param flags Input flags.
 * @return A pointer to the newly created input structure.
 */
REDUCT_API reduct_input_t* reduct_input_new(reduct_t* reduct, const char* buffer, reduct_size_t length,
    const char* path, reduct_input_flags_t flags);

/** @} */

#endif
