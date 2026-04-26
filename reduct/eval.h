#ifndef REDUCT_EVAL_H
#define REDUCT_EVAL_H 1

struct reduct_closure;

/**
 * @brief Reduct virtual machine evaluation.
 * @defgroup eval Evaluation
 * @file eval.h
 *
 * @{
 */

#include "core.h"
#include "function.h"
#include "handle.h"

#define REDUCT_EVAL_REGS_INITIAL 64      ///< The initial amount of registers.
#define REDUCT_EVAL_REGS_GROWTH_FACTOR 2 ///< The growth factor of the registers array.

#define REDUCT_EVAL_FRAMES_INITIAL 32      ///< The initial size of the frames array.
#define REDUCT_EVAL_FRAMES_GROWTH_FACTOR 2 ///< The growth factor of the frames array.

/**
 * @brief Reduct evaluation frame structure.
 * @struct reduct_eval_frame_t
 */
typedef struct reduct_eval_frame
{
    struct reduct_closure* closure; ///< The closure being evaluated.
    reduct_inst_t* ip;              ///< The current instruction pointer.
    reduct_uint32_t base;           ///< The base register, where the functions registers start.
    reduct_uint32_t prevRegCount;   ///< The previous register count to restore upon return.
} reduct_eval_frame_t;

/**
 * @brief Reduct evaluation state structure.
 * @struct reduct_eval_state_t
 */
typedef struct reduct_eval_state
{
    reduct_eval_frame_t* frames;
    reduct_uint32_t frameCount;
    reduct_uint32_t frameCapacity;
    reduct_handle_t* regs;
    reduct_uint32_t regCount;
    reduct_uint32_t regCapacity;
} reduct_eval_state_t;

/**
 * @brief Deinitialize an evaluation state structure.
 *
 * @param state The evaluation state to deinitialize.
 */
REDUCT_API void reduct_eval_state_deinit(reduct_eval_state_t* state);

/**
 * @brief Evaluates a compiled Reduct function.
 *
 * @param reduct The Reduct instance.
 * @param function The function to evaluate.
 * @return The result of the evaluation as a Reduct handle.
 */
REDUCT_API reduct_handle_t reduct_eval(reduct_t* reduct, reduct_function_t* function);

/**
 * @brief Calls a Reduct callable (closure or native) with arguments.
 *
 * @param reduct The Reduct instance.
 * @param callable The callable item handle.
 * @param argc The number of arguments.
 * @param argv Pointer to the arguments array.
 * @return The result of the call.
 */
REDUCT_API reduct_handle_t reduct_eval_call(reduct_t* reduct, reduct_handle_t callable, reduct_size_t argc, reduct_handle_t* argv);

/** @} */

#endif