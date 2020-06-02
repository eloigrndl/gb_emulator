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




//returns the correct interruption address, see cpu.h
#define ir_address(ir) \
    0x40 + (ir << 3)

//default structure of a conditional control structure, see cpu.h
#define control_pc_and(cpu, lu,body) \
    if(verify_cc(cpu, lu)){ \
        body; \
        cpu->idle_time += lu->xtra_cycles; \
    } else{ \
        cpu->PC += lu->bytes; \
    }\

#define BIT_MASK32(k) \
  ((size_t) k % 32 == 0 ? 0xFFFFFFFF : ((1 << ((k  % 32))) - 1)) //FIXME

#define BIT_MASK32_INV(k) \
    ~BIT_MASK32(32 - k)

#define LAST_FIELD32(pbv) \
    (pbv->nb_fields -1)

#define REMOVE_TAIL32(pbv) \
    if(pbv->size % 32 != 0){ \
         pbv->content[LAST_FIELD32(pbv)] &= BIT_MASK32(pbv->size); \
    } \



#define INIT_AND_PLUG(gb, X) \
    M_REQUIRE_NO_ERR(component_create(&(gameboy->components[gb->nb_components]), MEM_SIZE(X))); \
    M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[gb->nb_components++]), X ## _START, X ## _END)); \

#ifdef __cplusplus
#endif