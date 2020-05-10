#pragma once

/**
 * @file myMacros.h
 * @brief Additional macros for the PPS
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdint.h>
#include "error.h"
#include "cpu.h"
#include "opcode.h"

#ifdef __cplusplus
extern "C" {
#endif

// read value returned in case of an error, see bus.h
#define DEFAULT_READ_VALUE 0xFF


// additional flag constants, see cpu-alu.h
#define R16SP_FLAGS       CPU,   CLEAR, ALU,   ALU
#define UNCHANGED_FLAGS   CPU,   CPU,   CPU,   CPU
#define BIT_TEST_SRC      ALU,   CLEAR, SET,   CPU


// used in do_cpu_arithm to combine ALU flags, set F and set a chosen pair of registers, see cpu-alu.h
#define combine_flags_set_pair(cpu, reg, ...) \
    do { \
        M_EXIT_IF_ERR(cpu_combine_alu_flags(cpu, __VA_ARGS__)); \
        cpu_reg_pair_SP_set(cpu, reg, cpu->alu.value); \
    } while(0)


// size of a word in a GB, see cpu.h
#define WORD_SIZE 2


// additional getters and setters for single registers, see cpu-registers.h
#define cpu_A_get(cpu) \
    cpu_reg_get(cpu, REG_A_CODE)

#define cpu_B_get(cpu) \
    cpu_reg_get(cpu, REG_B_CODE)

#define cpu_C_get(cpu) \
    cpu_reg_get(cpu, REG_C_CODE)

#define cpu_D_get(cpu) \
    cpu_reg_get(cpu, REG_D_CODE)

#define cpu_E_get(cpu) \
    cpu_reg_get(cpu, REG_E_CODE)

#define cpu_H_get(cpu) \
    cpu_reg_get(cpu, REG_H_CODE)

#define cpu_L_get(cpu) \
    cpu_reg_get(cpu, REG_L_CODE)


#define cpu_A_set(cpu, value) \
    cpu_reg_set(cpu, REG_A_CODE, value)

#define cpu_B_set(cpu, value) \
    cpu_reg_set(cpu, REG_B_CODE, value)

#define cpu_C_set(cpu, value) \
    cpu_reg_set(cpu, REG_C_CODE, value)

#define cpu_D_set(cpu, value) \
    cpu_reg_set(cpu, REG_D_CODE, value)

#define cpu_E_set(cpu, value) \
    cpu_reg_set(cpu, REG_E_CODE, value)

#define cpu_H_set(cpu, value) \
    cpu_reg_set(cpu, REG_H_CODE, value)

#define cpu_L_set(cpu, value) \
    cpu_reg_set(cpu, REG_L_CODE, value)



// loads a value from the bus at the given index into register A, see cpu-alu.h
#define set_A_from_bus(cpu, idx) \
    cpu_A_set(cpu,  cpu_read_at_idx(cpu, idx))


/**
* @brief verify if a certain conditiion (cc) is verified or not
* @param  cpu cpu that stores the flags
* @param lu instruction to extract the condition
* @return the condition is verified or not
*/
int verify_cc(cpu_t* cpu, const instruction_t* lu);

/**
* @brief check if an interruption is pending if the same bit is 1 in IF and in IE
*/
uint8_t is_pending(cpu_t* cpu);

#ifdef __cplusplus
}
#endif
