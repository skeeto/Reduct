#ifndef REDUCT_HANDLE_IMPL_H
#define REDUCT_HANDLE_IMPL_H 1

#include "char.h"
#include "core.h"
#include "defs.h"
#include "eval.h"
#include "gc.h"
#include "handle.h"
#include "item.h"
#include "stringify.h"

REDUCT_API void reduct_handle_get_string_params(reduct_t* reduct, reduct_handle_t* handle, char** outStr, reduct_size_t* outLen)
{
    reduct_handle_ensure_item(reduct, handle);
    reduct_item_t* item = REDUCT_HANDLE_TO_ITEM(handle);
    if (item->type != REDUCT_ITEM_TYPE_ATOM)
    {
        REDUCT_ERROR_RUNTIME(reduct, "expected atom, got %s", reduct_item_type_str(item->type));
    }
    *outStr = item->atom.string;
    *outLen = item->length;
}

REDUCT_API void reduct_handle_ensure_item(reduct_t* reduct, reduct_handle_t* handle)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);
    REDUCT_ASSERT(handle != REDUCT_NULL);

    if (REDUCT_HANDLE_IS_ITEM(handle))
    {
        return;
    }

    if (REDUCT_HANDLE_IS_INT(handle))
    {
        reduct_atom_t* atom = reduct_atom_lookup_int(reduct, REDUCT_HANDLE_TO_INT(handle));
        *handle = REDUCT_HANDLE_FROM_ATOM(atom);
        return;
    }

    reduct_atom_t* atom = reduct_atom_lookup_float(reduct, REDUCT_HANDLE_TO_FLOAT(handle));
    *handle = REDUCT_HANDLE_FROM_ITEM(REDUCT_CONTAINER_OF(atom, reduct_item_t, atom));
}

REDUCT_API reduct_item_t* reduct_handle_item(reduct_t* reduct, reduct_handle_t* handle)
{
    REDUCT_ASSERT(handle != REDUCT_NULL);
    reduct_handle_ensure_item(reduct, handle);
    return REDUCT_HANDLE_TO_ITEM(handle);
}

REDUCT_API void reduct_handle_promote(struct reduct* reduct, reduct_handle_t* a, reduct_handle_t* b, reduct_promotion_t* out)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);
    REDUCT_ASSERT(a != REDUCT_NULL);
    REDUCT_ASSERT(b != REDUCT_NULL);
    REDUCT_ASSERT(out != REDUCT_NULL);

    if (REDUCT_HANDLE_IS_INT(a) && REDUCT_HANDLE_IS_INT(b))
    {
        out->type = REDUCT_PROMOTION_TYPE_INT;
        out->a.intVal = REDUCT_HANDLE_TO_INT(a);
        out->b.intVal = REDUCT_HANDLE_TO_INT(b);
        return;
    }

    if (REDUCT_HANDLE_IS_FLOAT(a) && REDUCT_HANDLE_IS_FLOAT(b))
    {
        out->type = REDUCT_PROMOTION_TYPE_FLOAT;
        out->a.floatVal = REDUCT_HANDLE_TO_FLOAT(a);
        out->b.floatVal = REDUCT_HANDLE_TO_FLOAT(b);
        return;
    }

    reduct_handle_ensure_item(reduct, a);
    reduct_handle_ensure_item(reduct, b);

    reduct_item_t* itemA = REDUCT_HANDLE_TO_ITEM(a);
    reduct_item_t* itemB = REDUCT_HANDLE_TO_ITEM(b);

    if ((REDUCT_HANDLE_GET_FLAGS(a) & REDUCT_ITEM_FLAG_FLOAT_SHAPED) ||
        (REDUCT_HANDLE_GET_FLAGS(b) & REDUCT_ITEM_FLAG_FLOAT_SHAPED))
    {
        out->type = REDUCT_PROMOTION_TYPE_FLOAT;
        if (REDUCT_HANDLE_GET_FLAGS(a) & REDUCT_ITEM_FLAG_FLOAT_SHAPED)
        {
            out->a.floatVal = itemA->atom.floatValue;
        }
        else
        {
            out->a.floatVal = (reduct_float_t)itemA->atom.integerValue;
        }

        if (REDUCT_HANDLE_GET_FLAGS(b) & REDUCT_ITEM_FLAG_FLOAT_SHAPED)
        {
            out->b.floatVal = itemB->atom.floatValue;
        }
        else
        {
            out->b.floatVal = (reduct_float_t)itemB->atom.integerValue;
        }
    }
    else if ((REDUCT_HANDLE_GET_FLAGS(a) & REDUCT_ITEM_FLAG_INT_SHAPED) &&
        (REDUCT_HANDLE_GET_FLAGS(b) & REDUCT_ITEM_FLAG_INT_SHAPED))
    {
        out->type = REDUCT_PROMOTION_TYPE_INT;
        out->a.intVal = itemA->atom.integerValue;
        out->b.intVal = itemB->atom.integerValue;
    }
    else
    {
        REDUCT_ERROR_RUNTIME(reduct, "unsupported operand type %s and %s", reduct_item_type_str(itemA->type),
            reduct_item_type_str(itemB->type));
    }
}

REDUCT_API reduct_bool_t reduct_handle_is_equal(reduct_t* reduct, reduct_handle_t* a, reduct_handle_t* b)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);
    REDUCT_ASSERT(a != REDUCT_NULL);
    REDUCT_ASSERT(b != REDUCT_NULL);

    if (*a == *b)
    {
        return REDUCT_TRUE;
    }

    reduct_handle_ensure_item(reduct, a);
    reduct_handle_ensure_item(reduct, b);

    return *a == *b;
}

typedef struct
{
    int group;
    reduct_bool_t isFloat;
    union {
        reduct_int64_t i;
        reduct_float_t f;
    } num;
    reduct_item_t* item;
} reduct_cmp_val_t;

static inline void reduct_handle_unpack(reduct_handle_t* handle, reduct_cmp_val_t* out)
{
    REDUCT_ASSERT(handle != REDUCT_NULL);
    REDUCT_ASSERT(out != REDUCT_NULL);

    if (REDUCT_HANDLE_IS_INT(handle))
    {
        out->group = 0;
        out->isFloat = REDUCT_FALSE;
        out->num.i = REDUCT_HANDLE_TO_INT(handle);
        out->item = REDUCT_NULL;
        return;
    } 
    
    if (REDUCT_HANDLE_IS_FLOAT(handle))
    {
        out->group = 0;
        out->isFloat = REDUCT_TRUE;
        out->num.f = REDUCT_HANDLE_TO_FLOAT(handle);
        out->item = REDUCT_NULL;
        return;
    }

    out->item = REDUCT_HANDLE_TO_ITEM(handle);
    if (out->item != REDUCT_NULL && out->item->type == REDUCT_ITEM_TYPE_LIST)
    {
        out->group = 2;
        return;
    }
    
    if (out->item != REDUCT_NULL && (REDUCT_HANDLE_GET_FLAGS(handle) & REDUCT_ITEM_FLAG_FLOAT_SHAPED))
    {
        out->group = 0;
        out->isFloat = REDUCT_TRUE;
        out->num.f = out->item->atom.floatValue;
        return;
    }

    if (out->item != REDUCT_NULL && (REDUCT_HANDLE_GET_FLAGS(handle) & REDUCT_ITEM_FLAG_INT_SHAPED))
    {
        out->group = 0;
        out->isFloat = REDUCT_FALSE;
        out->num.i = out->item->atom.integerValue;
        return;
    }

    out->group = 1;
}

REDUCT_API reduct_int64_t reduct_handle_compare(reduct_t* reduct, reduct_handle_t* a, reduct_handle_t* b)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);
    REDUCT_ASSERT(a != REDUCT_NULL);
    REDUCT_ASSERT(b != REDUCT_NULL);

    if (a == b || *a == *b)
    {
        return 0;
    }

    reduct_cmp_val_t va, vb;
    reduct_handle_unpack(a, &va);
    reduct_handle_unpack(b, &vb);

    if (va.group != vb.group)
    {
        return va.group - vb.group;
    }

    if (va.group == 0)
    {
        if (!va.isFloat && !vb.isFloat)
        {
            return (va.num.i < vb.num.i) ? -1 : ((va.num.i > vb.num.i) ? 1 : 0);
        }
        else if (va.isFloat && vb.isFloat)
        {
            return (va.num.f < vb.num.f) ? -1 : ((va.num.f > vb.num.f) ? 1 : 0);
        }

        reduct_float_t fa = va.isFloat ? va.num.f : (reduct_float_t)va.num.i;
        reduct_float_t fb = vb.isFloat ? vb.num.f : (reduct_float_t)vb.num.i;
        if (fa < fb)
        {
            return -1;
        }
        if (fa > fb)
        {
            return 1;
        }
        return va.isFloat ? 1 : -1;
    }
    else if (va.group == 1)
    {
        reduct_atom_t* atomA = va.item ? &va.item->atom : REDUCT_NULL;
        reduct_atom_t* atomB = vb.item ? &vb.item->atom : REDUCT_NULL;
        reduct_size_t lenA = atomA ? atomA->length : 0;
        reduct_size_t lenB = atomB ? atomB->length : 0;
        reduct_size_t minLen = lenA < lenB ? lenA : lenB;

        if (minLen > 0)
        {
            int cmp = memcmp(atomA->string, atomB->string, minLen);
            if (cmp != 0)
            {
                return cmp;
            }
        }

        return (reduct_int64_t)lenA - (reduct_int64_t)lenB;
    }

    reduct_list_t* listA = va.item ? &va.item->list : REDUCT_NULL;
    reduct_list_t* listB = vb.item ? &vb.item->list : REDUCT_NULL;
    reduct_size_t lenA = listA ? listA->length : 0;
    reduct_size_t lenB = listB ? listB->length : 0;
    reduct_size_t minLen = lenA < lenB ? lenA : lenB;

    if (minLen > 0)
    {
        reduct_list_iter_t iterA = REDUCT_LIST_ITER(listA);
        reduct_list_iter_t iterB = REDUCT_LIST_ITER(listB);
        reduct_handle_t itemA, itemB;

        for (reduct_size_t i = 0; i < minLen; i++)
        {
            reduct_list_iter_next(&iterA, &itemA);
            reduct_list_iter_next(&iterB, &itemB);
            reduct_int64_t cmp = reduct_handle_compare(reduct, &itemA, &itemB);
            if (cmp != 0)
            {
                return cmp;
            }
        }
    }

    return (reduct_int64_t)lenA - (reduct_int64_t)lenB;
}

REDUCT_API reduct_handle_t reduct_handle_nil(reduct_t* reduct)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    return REDUCT_HANDLE_FROM_ITEM(reduct->nilItem);
}

REDUCT_API reduct_handle_t reduct_handle_pi(reduct_t* reduct)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    return REDUCT_HANDLE_FROM_ITEM(reduct->piItem);
}

REDUCT_API reduct_handle_t reduct_handle_e(reduct_t* reduct)
{
    REDUCT_ASSERT(reduct != REDUCT_NULL);

    return REDUCT_HANDLE_FROM_ITEM(reduct->eItem);
}

#endif
