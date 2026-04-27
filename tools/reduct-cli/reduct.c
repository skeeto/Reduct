#define REDUCT_INLINE
#include "../../reduct/reduct.h"
#include "reduct_version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[0x10000];

int main(int argc, char **argv)
{
    int result = 0;
    int isSilent = 0;
    int shouldDump = 0;
    const char* evalExpr = NULL;
    const char* filename = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0)
        {
            isSilent = 1;
        }
        else if (strcmp(argv[i], "-e") == 0)
        {
            if (i + 1 < argc)
            {
                evalExpr = argv[++i];
            }
            else
            {
                fprintf(stderr, "error: -e requires an expression argument\n");
                result = 1;
                goto cleanup;
            }
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            shouldDump = 1;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("Reduct %s (%s)\n", REDUCT_VERSION_STRING, REDUCT_GIT_HASH);
            return 0;
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
            result = 1;
            goto cleanup;
        }
        else
        {
            filename = argv[i];
        }
    }

    if (filename == NULL && evalExpr == NULL)
    {
        fprintf(stderr, "Usage: %s [options] <filename>\n", argv[0]);
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -e <expr>      Evaluate the given expression\n");
        fprintf(stderr, "  -s, --silent   Do not print the evaluation result\n");
        fprintf(stderr, "  -d             Output the compiled bytecode (disassemble)\n");
        fprintf(stderr, "  -v, --version  Output version information\n");
        return 1;
    }

    reduct_t* reduct = NULL;

    reduct_error_t error = REDUCT_ERROR();
    if (REDUCT_ERROR_CATCH(&error))
    {
        reduct_error_print(&error, stderr);
        result = 1;
        goto cleanup;
    }

    reduct = reduct_new(&error);

    reduct_args_set(reduct, argc, argv);

    reduct_handle_t ast = REDUCT_HANDLE_NONE;
    if (evalExpr != NULL)
    {
        ast = reduct_parse(reduct, evalExpr, strlen(evalExpr), "<eval>");
    }
    else if (filename != NULL)
    {
        ast = reduct_parse_file(reduct, filename);
    }

    if (ast == REDUCT_HANDLE_NONE)
    {
        fprintf(stderr, "error: nothing to evaluate\n");
        result = 1;
        goto cleanup;
    }

    reduct_stdlib_register(reduct, REDUCT_STDLIB_ALL);

    reduct_function_t* function = reduct_compile(reduct, &ast);

    if (shouldDump)
    {
        reduct_disasm(reduct, function, stdout);
        goto cleanup;
    }

    reduct_handle_t eval = reduct_eval(reduct, function);
    if (isSilent)
    {
        goto cleanup;
    }

    reduct_stringify(reduct, &eval, buffer, sizeof(buffer));
    printf("%s%c", buffer, REDUCT_HANDLE_IS_ATOM(&eval) ? '\0' : '\n');

cleanup:
    reduct_free(reduct);

    return result;
}