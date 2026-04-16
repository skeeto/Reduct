#ifndef SCON_DISASM_API_H
#define SCON_DISASM_API_H 1

#include "core_api.h"
#include "compile_api.h"

/**
 * @brief Disassembles a compiled function and prints it to stdout.
 * 
 * @param scon The SCON structure.
 * @param function The compiled function to disassemble.
 */
SCON_API void scon_disasm(scon_t* scon, scon_function_t* function);

#endif