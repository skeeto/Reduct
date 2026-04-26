#include "item.h"
#ifndef REDUCT_GC_IMPL_H
#define REDUCT_GC_IMPL_H 1

#include "core.h"
#include "eval.h"
#include "gc.h"
#include "item.h"
#include "list.h"

static void reduct_gc_mark(reduct_t* reduct, reduct_item_t* item);

static void reduct_gc_mark_node(reduct_t* reduct, reduct_uint32_t shift, reduct_list_node_t* node)
{
    if (node == REDUCT_NULL)
    {
        return;
    }

    reduct_item_t* item = REDUCT_CONTAINER_OF(node, reduct_item_t, node);
    if (item->flags & REDUCT_ITEM_FLAG_GC_MARK)
    {
        return;
    }
    item->flags |= REDUCT_ITEM_FLAG_GC_MARK;

    if (shift == 0)
    {
        for (int i = 0; i < REDUCT_LIST_WIDTH; i++)
        {
            reduct_handle_t h = node->handles[i];
            if (REDUCT_HANDLE_IS_ITEM(&h))
            {
                reduct_gc_mark(reduct, REDUCT_HANDLE_TO_ITEM(&h));
            }
        }
    }
    else
    {
        for (int i = 0; i < REDUCT_LIST_WIDTH; i++)
        {
            reduct_gc_mark_node(reduct, shift - REDUCT_LIST_BITS, node->children[i]);
        }
    }
}

static void reduct_gc_mark_list(reduct_t* reduct, reduct_list_t* list)
{
    reduct_gc_mark_node(reduct, list->shift, list->root);
    reduct_gc_mark_node(reduct, 0, list->tail);
}

static void reduct_gc_mark(reduct_t* reduct, reduct_item_t* item)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    if (item == REDUCT_NULL || (item->flags & REDUCT_ITEM_FLAG_GC_MARK))
    {
        return;
    }

    item->flags |= REDUCT_ITEM_FLAG_GC_MARK;

    if (item->type == REDUCT_ITEM_TYPE_LIST)
    {
        reduct_gc_mark_list(reduct, &item->list);
    }
    else if (item->type == REDUCT_ITEM_TYPE_FUNCTION)
    {
        for (reduct_uint16_t i = 0; i < item->function.constantCount; i++)
        {
            if (item->function.constants[i].type == REDUCT_CONST_SLOT_ITEM)
            {
                reduct_gc_mark(reduct, item->function.constants[i].item);
            }
            else if (item->function.constants[i].type == REDUCT_CONST_SLOT_CAPTURE)
            {
                reduct_gc_mark(reduct, REDUCT_CONTAINER_OF(item->function.constants[i].capture, reduct_item_t, atom));
            }
        }
    }
    else if (item->type == REDUCT_ITEM_TYPE_CLOSURE)
    {
        reduct_gc_mark(reduct, REDUCT_CONTAINER_OF(item->closure.function, reduct_item_t, function));
        for (reduct_uint16_t i = 0; i < item->closure.function->constantCount; i++)
        {
            reduct_handle_t handle = item->closure.constants[i];
            if (REDUCT_HANDLE_IS_ITEM(&handle))
            {
                reduct_gc_mark(reduct, REDUCT_HANDLE_TO_ITEM(&handle));
            }
        }
    }
}

REDUCT_API void reduct_gc_if_needed(reduct_t* reduct)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    if (reduct->blocksAllocated >= reduct->gcThreshold)
    {
        reduct_gc(reduct);
        reduct->blocksAllocated = 0;
        reduct->gcThreshold = reduct->blocksAllocated + REDUCT_GC_THRESHOLD_INITIAL;
    }
}

REDUCT_API void reduct_gc(reduct_t* reduct)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    reduct_item_block_t* rootBlock = reduct->block;
    while (rootBlock != REDUCT_NULL)
    {
        for (int i = 0; i < REDUCT_ITEM_BLOCK_MAX; i++)
        {
            reduct_item_t* item = &rootBlock->items[i];
            if (item->type != REDUCT_ITEM_TYPE_NONE && item->retainCount > 0)
            {
                reduct_gc_mark(reduct, item);
            }
        }
        rootBlock = rootBlock->next;
    }

    if (reduct->evalState != REDUCT_NULL)
    {
        for (reduct_uint32_t i = 0; i < reduct->evalState->regCount; i++)
        {
            reduct_handle_t child = reduct->evalState->regs[i];
            if (REDUCT_HANDLE_IS_ITEM(&child))
            {
                reduct_gc_mark(reduct, REDUCT_HANDLE_TO_ITEM(&child));
            }
        }
        for (reduct_uint32_t i = 0; i < reduct->evalState->frameCount; i++)
        {
            reduct_closure_t* closure = reduct->evalState->frames[i].closure;
            reduct_gc_mark(reduct, REDUCT_CONTAINER_OF(closure, reduct_item_t, closure));
        }
    }

    reduct->freeList = REDUCT_NULL;

    reduct_item_block_t* block = reduct->block;
    while (block != REDUCT_NULL)
    {
        for (int i = 0; i < REDUCT_ITEM_BLOCK_MAX; i++)
        {
            reduct_item_t* item = &block->items[i];
            if (item->flags & REDUCT_ITEM_FLAG_GC_MARK)
            {
                item->flags &= ~REDUCT_ITEM_FLAG_GC_MARK;
            }
            else
            {
                reduct_item_free(reduct, item);
            }
        }
        block = block->next;
    }
}

#endif
