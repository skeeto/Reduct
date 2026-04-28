#ifndef REDUCT_GC_H
#define REDUCT_GC_H 1

#include "core.h"
#include "handle.h"
#include "item.h"

/**
 * @file gc.h
 * @brief Garbage collection
 * @defgroup gc Garbage Collection
 *
 * @todo The Garbage collector really needs to be optimized.
 *
 * @{
 */

/**
 * @brief Run the garbage collector.
 *
 * @param reduct The Reduct structure.
 */
REDUCT_API void reduct_gc(reduct_t* reduct);

/**
 * @brief Optionally run the garbage collector if the number of allocated blocks exceeds the threshold.

 * @param reduct The Reduct structure.
 */
REDUCT_API void reduct_gc_if_needed(reduct_t* reduct);

/**
 * @brief Retain an item, preventing it from being collected by the GC.
 *
 * @param reduct The Reduct structure.
 * @param item The item to retain, must be a item.
 */
#define REDUCT_GC_RETAIN(_reduct, _handle) \
    do \
    { \
        (void)(_reduct); \
        reduct_handle_t __handle = (_handle); \
        if (REDUCT_HANDLE_IS_ITEM(&__handle)) \
        { \
            reduct_item_t* __item = REDUCT_HANDLE_TO_ITEM(&__handle); \
            __item->retainCount++; \
        } \
    } while (0)

/**
 * @brief Retain a item, preventing it from being collected by the GC.
 *
 * @param reduct The Reduct structure.
 * @param item The item to retain.
 */
#define REDUCT_GC_RETAIN_ITEM(_reduct, _item) \
    do \
    { \
        (void)(_reduct); \
        reduct_item_t* __item = (_item); \
        __item->retainCount++; \
    } while (0)

/**
 * @brief Release a previously retained item, allowing it to be collected by the GC.
 *
 * @param reduct The Reduct structure.
 * @param item The item to release.
 */
#define REDUCT_GC_RELEASE(_reduct, _handle) \
    do \
    { \
        (void)(_reduct); \
        reduct_handle_t __handle = (_handle); \
        if (REDUCT_HANDLE_IS_ITEM(&__handle)) \
        { \
            reduct_item_t* __item = REDUCT_HANDLE_TO_ITEM(&__handle); \
            __item->retainCount--; \
        } \
    } while (0)

/** @} */

#endif
