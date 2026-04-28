#ifndef REDUCT_ATOM_H
#define REDUCT_ATOM_H 1

#include "defs.h"
#include "intrinsic.h"
#include "native.h"

struct reduct;

/**
 * @brief Atom representation and operations.
 * @defgroup atom Atoms
 * @file atom.h
 *
 * Atoms represent all strings within a Reduct expression, as such it also represents anything that a string can be,
 * including integers, floats and intrinsics.
 *
 * ## Interning
 *
 * To improve performance, we "intern" all atoms. Meaning that instead of needing to use `strcmp()` or similar to
 * compare atoms, we store all atoms in a hash map.
 *
 * When a new string is encountered, we look it up in the map. As such, any instance of the same string will always be
 * represented by the same `reduct_atom_t` structure. Turning a string comparison into a pointer comparison and avoiding
 * redundant parsing of numeric or intrinsic values.
 *
 * @see [Wikipedia String Interning](https://en.wikipedia.org/wiki/String_interning)
 *
 * @{
 */

#define REDUCT_ATOM_SMALL_MAX 15 ///< The maximum length of a small atom.

/**
 * @brief Atom lookup flags.
 */
typedef enum
{
    REDUCT_ATOM_LOOKUP_NONE = 0,       ///< No flags.
    REDUCT_ATOM_LOOKUP_QUOTED = 1 << 0 ///< Atom should be explicitly quoted.
} reduct_atom_lookup_flags_t;

/**
 * @brief Atom structure.
 * @struct reduct_atom_t
 */
typedef struct reduct_atom
{
    reduct_uint32_t length; ///< The length of the string (must be first, check the `reduct_item_t` structure).
    reduct_uint32_t hash;   ///< The hash of the string.
    char small[REDUCT_ATOM_SMALL_MAX]; ///< The small string buffer.
    reduct_intrinsic_t intrinsic;      ///< Cached intrinsic, item must have `REDUCT_ITEM_FLAG_INTRINSIC`.
    char* string;                      ///< Pointer to the string.
    union {
        reduct_int64_t integerValue; ///< Pre-computed integer value, item must have `REDUCT_ITEM_FLAG_INT_SHAPED`.
        reduct_float_t floatValue;   ///< Pre-computed float value, item must have `REDUCT_ITEM_FLAG_FLOAT_SHAPED`.
        reduct_native_fn native;     ///< Native function, item must have `REDUCT_ITEM_FLAG_NATIVE`.
    };
    struct reduct_atom* next; ///< Pointer to the next atom in the hash map.
} reduct_atom_t;

#define REDUCT_FNV_PRIME 16777619U    ///< FNV-1a 32-bit prime.
#define REDUCT_FNV_OFFSET 2166136261U ///< FNV-1a 32-bit offset basis.

/**
 * @brief Hash a string.
 *
 * @param str The string to hash.
 * @param len The length of the string.
 * @return The hash of the string.
 */
static inline REDUCT_ALWAYS_INLINE reduct_uint32_t reduct_hash(const char* str, reduct_size_t len)
{
    reduct_uint32_t hash = REDUCT_FNV_OFFSET;
    for (reduct_size_t i = 0; i < len; i++)
    {
        hash ^= (unsigned char)str[i];
        hash *= REDUCT_FNV_PRIME;
    }
    return hash;
}

/**
 * @brief Initialize an atom.
 *
 * @param atom Pointer to the atom to initialize.
 */
static inline void reduct_atom_init(reduct_atom_t* atom)
{
    REDUCT_ASSERT(atom != REDUCT_NULL);
    atom->length = 0;
    atom->next = REDUCT_NULL;
    atom->hash = 0;
}

/**
 * @brief Deinitialize an atom.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param atom Pointer to the atom to deinitialize.
 */
REDUCT_API void reduct_atom_deinit(struct reduct* reduct, reduct_atom_t* atom);

/**
 * @brief Check if an atom is equal to a string.
 *
 * @param atom Pointer to the atom.
 * @param str The string to compare.
 * @param len The length of the string.
 * @return `REDUCT_TRUE` if the atom is equal to the string, `REDUCT_FALSE` otherwise.
 */
static inline REDUCT_ALWAYS_INLINE reduct_bool_t reduct_atom_is_equal(reduct_atom_t* atom, const char* str,
    reduct_size_t len)
{
    REDUCT_ASSERT(atom != REDUCT_NULL);
    REDUCT_ASSERT(str != REDUCT_NULL);

    if (atom->length != len)
    {
        return REDUCT_FALSE;
    }

    return REDUCT_MEMCMP(str, atom->string, len) == 0;
}

/**
 * @brief Lookup an atom by integer value.
 *
 * Will create a new atom if it does not exist.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param value The integer value.
 * @return A pointer to the atom.
 */
REDUCT_API reduct_atom_t* reduct_atom_lookup_int(struct reduct* reduct, reduct_int64_t value);

/**
 * @brief Lookup an atom by float value.
 *
 * Will create a new atom if it does not exist.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param value The float value.
 * @return A pointer to the atom.
 */
REDUCT_API reduct_atom_t* reduct_atom_lookup_float(struct reduct* reduct, reduct_float_t value);

/**
 * @brief Lookup an atom in the Reduct structure.
 *
 * Will create a new atom if it does not exist.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param str The string to lookup.
 * @param len The length of the string.
 * @param flags Lookup flags to alter the interning behavior.
 * @return A pointer to the atom.
 */
REDUCT_API reduct_atom_t* reduct_atom_lookup(struct reduct* reduct, const char* str, reduct_size_t len,
    reduct_atom_lookup_flags_t flags);

/**
 * @brief Normalize an atom, determining its shape and parsing escape sequences.
 *
 * @warning Should only be called on atoms stored in a `reduct_item_t`.
 *
 * @param reduct Pointer to the Reduct structure.
 * @param atom Pointer to the atom to normalize.
 */
REDUCT_API void reduct_atom_normalize(struct reduct* reduct, reduct_atom_t* atom);

/** @} */

#endif