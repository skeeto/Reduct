#ifndef REDUCT_ERROR_IMPL_H
#define REDUCT_ERROR_IMPL_H 1

#include "error.h"
#include "eval.h"
#include "item.h"

static inline reduct_size_t reduct_error_get_region_length(const char* ptr, const char* end)
{
    if (ptr >= end)
    {
        return 0;
    }

    if (*ptr == '(')
    {
        reduct_size_t depth = 0;
        reduct_bool_t inString = REDUCT_FALSE;
        const char* current = ptr;
        while (current < end)
        {
            if (*current == '\\' && current + 1 < end)
            {
                current += 2;
                continue;
            }
            if (*current == '"')
            {
                inString = !inString;
            }
            else if (!inString)
            {
                if (*current == '(')
                {
                    depth++;
                }
                else if (*current == ')')
                {
                    depth--;
                    if (depth == 0)
                    {
                        current++;
                        break;
                    }
                }
            }
            current++;
        }
        return (reduct_size_t)(current - ptr);
    }
    else if (*ptr == '"')
    {
        const char* current = ptr + 1;
        while (current < end)
        {
            if (*current == '\\' && current + 1 < end)
            {
                current += 2;
                continue;
            }
            if (*current == '"')
            {
                current++;
                break;
            }
            current++;
        }
        return (reduct_size_t)(current - ptr);
    }
    else
    {
        const char* current = ptr;
        while (current < end && *current != ' ' && *current != '\t' && *current != '\n' && *current != '\r' &&
            *current != '(' && *current != ')')
        {
            current++;
        }
        return (reduct_size_t)(current - ptr);
    }
}

REDUCT_API void reduct_error_print(reduct_error_t* error, reduct_file_t file)
{
    REDUCT_ASSERT(error != REDUCT_NULL);

    reduct_size_t row;
    reduct_size_t column;
    reduct_error_get_row_column(error, &row, &column);

    if (error->path != REDUCT_NULL)
    {
        REDUCT_FPRINTF(file, "%s:%zu:%zu: error: %s\n", error->path, row, column, error->message);
    }
    else
    {
        REDUCT_FPRINTF(file, "error: %s\n", error->message);
    }

    if (error->input != REDUCT_NULL)
    {
        const char* lineStart = error->input + error->index;
        while (lineStart > error->input && *(lineStart - 1) != '\n')
        {
            lineStart--;
        }

        const char* lineEnd = error->input + error->index;
        while (lineEnd < error->input + error->inputLength && *lineEnd != '\n' && *lineEnd != '\r')
        {
            lineEnd++;
        }

        reduct_size_t lineLen = (reduct_size_t)(lineEnd - lineStart);
        REDUCT_FPRINTF(file, " %4zu | %.*s\n", row, (int)lineLen, lineStart);
        REDUCT_FPRINTF(file, "      | ");

        for (reduct_size_t i = 0; i < column - 1; i++)
        {
            REDUCT_FWRITE(" ", 1, 1, file);
        }
        reduct_size_t indicatorLen = error->regionLength > 0 ? error->regionLength : 1;
        for (reduct_size_t i = 0; i < indicatorLen; i++)
        {
            REDUCT_FWRITE("^", 1, 1, file);
        }
        REDUCT_FWRITE("\n", 1, 1, file);
    }
    else
    {
        REDUCT_FWRITE("\n", 1, 1, file);
    }
}

REDUCT_API void reduct_error_get_row_column(reduct_error_t* error, reduct_size_t* row, reduct_size_t* column)
{
    REDUCT_ASSERT(error != REDUCT_NULL);
    REDUCT_ASSERT(row != REDUCT_NULL);
    REDUCT_ASSERT(column != REDUCT_NULL);

    *row = 1;
    *column = 1;

    if (error->input == REDUCT_NULL)
    {
        return;
    }

    for (reduct_size_t i = 0; i < error->index; i++)
    {
        if (error->input[i] == '\n')
        {
            (*row)++;
            *column = 1;
        }
        else
        {
            (*column)++;
        }
    }
}

REDUCT_API void reduct_error_set(reduct_error_t* error, const char* path, const char* input, reduct_size_t inputLength,
    reduct_size_t regionLength, reduct_size_t position, reduct_error_type_t type, const char* message, ...)
{
    REDUCT_ASSERT(error != REDUCT_NULL);
    REDUCT_ASSERT(message != REDUCT_NULL);

    error->path = path;
    error->input = input;
    error->inputLength = inputLength;
    error->regionLength = regionLength;
    error->index = position;

    reduct_va_list args;
    REDUCT_VA_START(args, message);
    REDUCT_VSNPRINTF(error->message, REDUCT_ERROR_MAX_LEN, message, args);
    REDUCT_VA_END(args);
}

REDUCT_API void reduct_error_get_item_params(struct reduct_item* item, const char** path, const char** input,
    reduct_size_t* inputLength, reduct_size_t* regionLength, reduct_size_t* position)
{
    if (item != REDUCT_NULL && item->input != REDUCT_NULL)
    {
        *path = item->input->path;
        *input = item->input->buffer;
        *inputLength = item->input->end - item->input->buffer;
        *regionLength = reduct_error_get_region_length(item->input->buffer + item->position, item->input->end);
        if (*regionLength == 0)
        {
            *regionLength = 1;
        }

        *position = item->position;
    }
    else
    {
        *path = REDUCT_NULL;
        *input = REDUCT_NULL;
        *inputLength = 0;
        *regionLength = 0;
        *position = 0;
    }
}

REDUCT_API void reduct_error_throw_runtime(struct reduct* reduct, const char* message, ...)
{
    const char* path = REDUCT_NULL;
    const char* input = REDUCT_NULL;
    reduct_size_t inputLength = 0;
    reduct_size_t regionLength = 0;
    reduct_size_t position = 0;

    if (reduct->evalState != REDUCT_NULL && reduct->evalState->frameCount > 0)
    {
        reduct_eval_frame_t* frame = &reduct->evalState->frames[reduct->evalState->frameCount - 1];
        if (frame->closure != REDUCT_NULL && frame->closure->function != REDUCT_NULL)
        {
            reduct_function_t* func = frame->closure->function;
            reduct_size_t instIndex = frame->ip > func->insts ? (reduct_size_t)(frame->ip - func->insts - 1) : 0;
            if (instIndex < func->instCount && func->positions != REDUCT_NULL)
            {
                position = func->positions[instIndex];
            }

            reduct_item_t* funcItem = REDUCT_CONTAINER_OF(func, reduct_item_t, function);
            if (funcItem->input != REDUCT_NULL)
            {
                path = funcItem->input->path;
                input = funcItem->input->buffer;
                inputLength = (reduct_size_t)(funcItem->input->end - funcItem->input->buffer);
                regionLength = reduct_error_get_region_length(input + position, funcItem->input->end);
                if (regionLength == 0)
                {
                    regionLength = 1;
                }
            }
        }
    }

    reduct_va_list args;
    REDUCT_VA_START(args, message);
    char formattedMessage[REDUCT_ERROR_MAX_LEN];
    REDUCT_VSNPRINTF(formattedMessage, REDUCT_ERROR_MAX_LEN, message, args);
    REDUCT_VA_END(args);

    reduct_error_set(reduct->error, path, input, inputLength, regionLength, position, REDUCT_ERROR_TYPE_RUNTIME, "%s",
        formattedMessage);
    REDUCT_LONGJMP(reduct->error->jmp, REDUCT_TRUE);
}

REDUCT_API void reduct_error_check_arity(reduct_t* reduct, reduct_size_t argc, reduct_size_t expected, const char* name)
{
    if (REDUCT_UNLIKELY(argc != expected))
    {
        REDUCT_ERROR_RUNTIME(reduct, "%s expects exactly %zu argument(s), got %zu", name, expected, (reduct_size_t)argc);
    }
}

REDUCT_API void reduct_error_check_min_arity(reduct_t* reduct, reduct_size_t argc, reduct_size_t min, const char* name)
{
    if (REDUCT_UNLIKELY(argc < min))
    {
        REDUCT_ERROR_RUNTIME(reduct, "%s expects at least %zu argument(s), got %zu", name, (reduct_size_t)min, (reduct_size_t)argc);
    }
}

REDUCT_API void reduct_error_check_arity_range(reduct_t* reduct, reduct_size_t argc, reduct_size_t min, reduct_size_t max,
    const char* name)
{
    if (REDUCT_UNLIKELY(argc < min || argc > max))
    {
        REDUCT_ERROR_RUNTIME(reduct, "%s expects between %zu and %zu argument(s), got %zu", name, (reduct_size_t)min, (reduct_size_t)max, (reduct_size_t)argc);
    }
}

#endif
