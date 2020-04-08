/**
 * @file cpu-registers.c
 * @brief Game Boy CPU simulation, register part
 *
 * @date 2019
 */

#include "error.h"
#include "cpu-storage.h" // cpu_read_at_HL
#include "cpu-registers.h" // cpu_BC_get
#include "gameboy.h" // REGISTER_START
#include "util.h"
#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

// ==== see cpu-storage.h ========================================
data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr)
{
    if(cpu == NULL || cpu->bus == NULL)
        return  0;   /// TODO wrong type : error_code  != addr_t

    data_t data = 0;
    bus_read(*(cpu->bus), addr, &data);                         
    return data;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr)
{
     if(cpu == NULL || cpu->bus == NULL)
        return  0;   /// TODO wrong type : error_code  != addr_t
    addr_t data = 0;
    bus_read16(*cpu->bus, addr, &data);  
                
    return data;
}

// ==== see cpu-storage.h ========================================
int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data)
{
    if(cpu == NULL || cpu->bus == NULL)
        return  ERR_BAD_PARAMETER;

    error_code e = bus_write(*(cpu->bus), addr, data);
    return e;
}

// ==== see cpu-storage.h ========================================
int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16)
{
   if(cpu == NULL || cpu->bus == NULL)
        return  ERR_BAD_PARAMETER;

    error_code e = bus_write16(*(cpu->bus), addr, data16);
    return e;
}

// ==== see cpu-storage.h ========================================
int cpu_SP_push(cpu_t* cpu, addr_t data16)
{
    if(cpu == NULL || cpu->bus == NULL) 
        return ERR_BAD_PARAMETER;
    
    cpu->SP -= 2;

    error_code e = cpu_write16_at_idx(cpu, cpu->SP, data16);
    return e;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_SP_pop(cpu_t* cpu)
{
     if(cpu == NULL || cpu->bus == NULL) 
        return 0;               //FIXME: do we have to return an error_code in case of nullPointer
    
    addr_t data = 0;
    data = cpu_read16_at_idx(cpu, cpu->SP);
    cpu->SP +=2;
    return data;
}

// ==== see cpu-storage.h ========================================


//TODO: need to verify args / errors
int cpu_dispatch_storage(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);

    switch (lu->family) {
        case LD_A_BCR:
            cpu_A_set(cpu, cpu_BC_get(cpu));
            break;

        case LD_A_CR:
            cpu_A_set(cpu, 0xFF00 + cpu_C_get(cpu));
            break;

        case LD_A_DER:
            cpu_A_set(cpu, cpu_DE_get(cpu));
            break;

        case LD_A_HLRU:
            cpu_A_set(cpu, cpu_HL_get(cpu)); 
            cpu_HL_set(cpu, cpu_HL_get(cpu) + cpu_extract_HL_increment(lu -> opcode));
            break;

        case LD_A_N16R:
            cpu_A_set(cpu, cpu_read_data16_after_opcode(cpu));      // TODO: maybe define macro for this
            break;

        case LD_A_N8R:
            cpu_read_data16_after_opcode(cpu);
            cpu_A_set(cpu, cpu_read_data16_after_opcode( cpu)); 
            
            break;
            
        // ============= inversed order from here
        case LD_BCR_A:
            error_code e = cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_A_get(cpu));
            return e;

        case LD_CR_A:
            error_code e = cpu_write_at_idx(cpu, 0xFF00 + cpu_C_get(cpu), cpu_A_get(cpu));
            return e;

        case LD_DER_A:
            error_code e = cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_A_get(cpu));
            return e;

        case LD_HLRU_A:
            error_code e = cpu_write_at_idx(cpu, cpu_HL_get(cpu), cpu_A_get(cpu));                 // TODO maybe make macro for HL
            cpu_HL_set(cpu, cpu_HL_get(cpu) + cpu_extract_HL_increment(lu->opcode));
            return e;

        case LD_HLR_N8:
            error_code e =  cpu_write_at_idx(cpu, cpu_HL_get(cpu), cpu_read_data_after_opcode(cpu));
            return e;

        case LD_HLR_R8:
            error_code e = cpu_write_at_idx(cpu, cpu_HL_get(cpu), cpu_get(cpu, extract_reg(lu->opcode, 0)));
            return e;

        case LD_N16R_A:
            error_code e = cpu_write16_at_idx(cpu, cpu_read_data16_after_opcode(cpu), cpu_A_get(cpu));
            return e;

        case LD_N16R_SP:
            error_code e = cpu_write16_at_idx(cpu, cpu_read_data16_after_opcode(cpu), cpu_SP_get(cpu));
            return e;

        case LD_N8R_A:
            error_code e = cpu_write_at_idx(cpu, 0xFF00 + cpu_read_data_after_opcode(cpu), cpu_A_get(cpu));//BUS[0xFF00 + n] = A
            return e;

        case LD_R16SP_N16:
            cpu_reg_pair_set(cpu, extract_reg(lu->opcode, 5),cpu_read_data16_after_opcode(cpu));
            break;

        case LD_R8_HLR:
            error_code e = cpu_write_at_idx(cpu, extract_reg(lu -> opcode, 3), cpu_read_at_HL(cpu));//r = BUS[HL]
            break;

        case LD_R8_N8:
            cpu_reg_set(cpu, extract_reg(lu->opcode,3), cpu_read_data_after_opcode(cpu));
            break;

        case LD_R8_R8: 
            cpu_reg_set(cpu, extract_reg(lu->opcode, 3), extract_reg(lu->opcode, 0)); //TODO: need to verify that r != s?
            break;

        case LD_SP_HL:
            cpu_reg_pair_SP_get(cpu, cpu_read_at_HL(cpu));
            break;

        case POP_R16:
            error_code e = cpu_write16_at_idx(cpu, extract_reg(lu -> opcode, 4), cpu_SP_pop(cpu));
            return e;

        case PUSH_R16:
            cpu_SP_push(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode, 4)));
            break;

        default:
            fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
            return ERR_INSTR;
            break;
    } // switch

    return ERR_NONE;
}
