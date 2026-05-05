#define REDUCT_INLINE
#include "reduct/reduct.h"

#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    reduct_t* reduct = NULL;

    reduct_error_t error = REDUCT_ERROR();

    if (REDUCT_ERROR_CATCH(&error))
    {
        reduct_free(reduct);
        return 0;
    }

    reduct = reduct_new(&error);

    // Skip SYSTEM so the fuzzer cannot touch the filesystem, env, args, or import.
    reduct_stdlib_register(reduct, (reduct_stdlib_sets_t)(REDUCT_STDLIB_ALL & ~REDUCT_STDLIB_SYSTEM));

    reduct_handle_t parsed = reduct_parse(reduct, (const char*)data, size, "<fuzz>");
    reduct_function_t* function = reduct_compile(reduct, &parsed);
    (void)reduct_eval(reduct, function);

    reduct_free(reduct);
    return 0;
}
