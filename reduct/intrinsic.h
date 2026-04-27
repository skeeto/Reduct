#ifndef REDUCT_INTRINSIC_H
#define REDUCT_INTRINSIC_H 1

#include "defs.h"
#include "native.h"

struct reduct;
struct reduct_compiler;
struct reduct_item;
struct reduct_expr;

/**
 * @brief Reduct intrinsic management.
 * @defgroup intrinsic Intrinsics
 * @file intrinsic.h
 *
 * A intrinsic is an atom recognized by the bytecode compiler as a special form or built-in operator, resulting in it
 * emitting specific bytecode instructions for each intrinsic.
 *
 * This is in contrast to a "native" which will be invoked as a standard function call at runtime.
 *
 * @{
 */

/**
 * @brief Reduct intrinsic types.
 */
typedef reduct_uint8_t reduct_intrinsic_t;

#define REDUCT_INTRINSIC_NONE 0   ///< None
#define REDUCT_INTRINSIC_QUOTE 1  ///< Quote
#define REDUCT_INTRINSIC_LIST 2   ///< List
#define REDUCT_INTRINSIC_DO 3     ///< Do
#define REDUCT_INTRINSIC_LAMBDA 4 ///< Lambda
#define REDUCT_INTRINSIC_THREAD 5 ///< Thread
#define REDUCT_INTRINSIC_DEF 6    ///< Def
#define REDUCT_INTRINSIC_IF 7     ///< If
#define REDUCT_INTRINSIC_COND 8  ///< Cond
#define REDUCT_INTRINSIC_MATCH 9 ///< Match
#define REDUCT_INTRINSIC_AND 10   ///< And
#define REDUCT_INTRINSIC_OR 11    ///< Or
#define REDUCT_INTRINSIC_NOT 12   ///< Not
#define REDUCT_INTRINSIC_ADD 13   ///< Add
#define REDUCT_INTRINSIC_SUB 14   ///< Sub
#define REDUCT_INTRINSIC_MUL 15   ///< Mul
#define REDUCT_INTRINSIC_DIV 16   ///< Div
#define REDUCT_INTRINSIC_MOD 17   ///< Mod
#define REDUCT_INTRINSIC_INC 18   ///< Inc
#define REDUCT_INTRINSIC_DEC 19   ///< Dec
#define REDUCT_INTRINSIC_BAND 20  ///< Bitwise And
#define REDUCT_INTRINSIC_BOR 21   ///< Bitwise Or
#define REDUCT_INTRINSIC_BXOR 22  ///< Bitwise Xor
#define REDUCT_INTRINSIC_BNOT 23  ///< Bitwise Not
#define REDUCT_INTRINSIC_SHL 24   ///< Bitwise Shift Left
#define REDUCT_INTRINSIC_SHR 25   ///< Bitwise Shift Right
#define REDUCT_INTRINSIC_EQ 26    ///< Equal
#define REDUCT_INTRINSIC_NEQ 27   ///< Not Equal
#define REDUCT_INTRINSIC_SEQ 28   ///< Strict Equal
#define REDUCT_INTRINSIC_SNEQ 29  ///< Strict Not Equal
#define REDUCT_INTRINSIC_LT 30    ///< Less
#define REDUCT_INTRINSIC_LE 31    ///< Less Equal
#define REDUCT_INTRINSIC_GT 32    ///< Greater
#define REDUCT_INTRINSIC_GE 33    ///< Greater Equal
#define REDUCT_INTRINSIC_MAX 34   ///< The amount of intrinsics

/**
 * @brief Reduct intrinsic handler function type.
 */
typedef void (*reduct_intrinsic_handler_t)(struct reduct_compiler* compiler, struct reduct_item* expr, struct reduct_expr* out);

/**
 * @brief Reduct intrinsic handler functions array.
 */
extern reduct_intrinsic_handler_t reductIntrinsicHandlers[REDUCT_INTRINSIC_MAX];

/**
 * @brief Reduct intrinsic native functions array.
 */
extern reduct_native_fn reductIntrinsicNatives[REDUCT_INTRINSIC_MAX];

/**
 * @brief Reduct intrinsic names array.
 */
extern const char* reductIntrinsics[REDUCT_INTRINSIC_MAX];

/**
 * @brief Registers all Reduct intrinsics with the given Reduct instance.
 *
 * @param reduct The Reduct instance to register intrinsics with.
 */
REDUCT_API void reduct_intrinsic_register_all(struct reduct* reduct);

/**
 * @brief Compiles a block of expressions (like a `do` block).
 *
 * @param compiler The compiler context.
 * @param list The list containing the expressions.
 * @param startIdx The index to start compiling from.
 * @param out The output expression.
 */
void reduct_intrinsic_block_generic(struct reduct_compiler* compiler, struct reduct_item* list, reduct_uint32_t startIdx,
    struct reduct_expr* out);

#endif
