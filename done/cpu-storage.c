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
#include "myMacros.h"   // WORD_SIZE, set_A_from_bus

#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

// ==== see cpu-storage.h ========================================
data_t cpu_read_at_idx(const cpu_t* cpu, addr_t addr)
{
    if(cpu == NULL || cpu->bus == NULL)
        return  DEFAULT_READ_VALUE;

    data_t data = 0;
    M_REQUIRE_NO_ERR(bus_read(*(cpu->bus), addr, &data)); 
    return data;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_read16_at_idx(const cpu_t* cpu, addr_t addr)
{
    if(cpu == NULL || cpu->bus == NULL)
        return DEFAULT_READ_VALUE;
        
    addr_t data = 0;
    bus_read16(*cpu->bus, addr, &data);   
    return data;
}

// ==== see cpu-storage.h ========================================
int cpu_write_at_idx(cpu_t* cpu, addr_t addr, data_t data)
{   
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);
    
    M_REQUIRE_NO_ERR(bus_write(*(cpu->bus), addr, data));
    cpu->write_listener = addr; 
    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
int cpu_write16_at_idx(cpu_t* cpu, addr_t addr, addr_t data16)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    M_REQUIRE_NO_ERR(bus_write16(*(cpu->bus), addr, data16));
    cpu->write_listener = addr; 
    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
int cpu_SP_push(cpu_t* cpu, addr_t addr)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);
    
    uint16_t temp = cpu->SP - WORD_SIZE; 
    
    M_REQUIRE_NO_ERR(cpu_write16_at_idx(cpu, temp, addr));
    cpu->SP -= WORD_SIZE; 

    return ERR_NONE;
}

// ==== see cpu-storage.h ========================================
addr_t cpu_SP_pop(cpu_t* cpu)
{
    if(cpu == NULL || cpu->bus == NULL) 
        return DEFAULT_READ_VALUE;
    
    addr_t data = cpu_read16_at_idx(cpu, cpu->SP);
    cpu->SP += WORD_SIZE;
    return data;
}

// ==== see cpu-storage.h ========================================

int cpu_dispatch_storage(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);


    switch (lu->family) {
        case LD_A_BCR: 
            set_A_from_bus(cpu, cpu_BC_get(cpu)); //TODO: simplify reading from bus with macros
            break;

        case LD_A_CR:
            set_A_from_bus(cpu, REGISTERS_START + cpu_C_get(cpu));
            break;

        case LD_A_DER:
            set_A_from_bus(cpu, cpu_DE_get(cpu));
            break;

        case LD_A_HLRU:
            set_A_from_bus(cpu, cpu_HL_get(cpu)); 
            cpu->HL += extract_HL_increment(lu->opcode);
            break;

        case LD_A_N16R:
            set_A_from_bus(cpu, cpu_read_addr_after_opcode(cpu));      // TODO: maybe define macro for this
            break;

        case LD_A_N8R:
            set_A_from_bus(cpu, REGISTERS_START + cpu_read_data_after_opcode(cpu)); 
            break;  
            
        // ============= inversed order from here
        case LD_BCR_A:
            M_REQUIRE_NO_ERR(cpu_write_at_idx(cpu, cpu_BC_get(cpu), cpu_A_get(cpu)));
            break;
            
        case LD_CR_A:
            M_REQUIRE_NO_ERR(cpu_write_at_idx(cpu, REGISTERS_START + cpu_C_get(cpu), cpu_A_get(cpu)));
            break; //FIXME: need to add a break?

        case LD_DER_A:
            M_REQUIRE_NO_ERR(cpu_write_at_idx(cpu, cpu_DE_get(cpu), cpu_A_get(cpu)));
            break;

        case LD_HLRU_A:        
            M_REQUIRE_NO_ERR(cpu_write_at_HL(cpu, cpu_A_get(cpu)));
            cpu->HL += extract_HL_increment(lu->opcode);
            break;

        case LD_HLR_N8:
            M_REQUIRE_NO_ERR(cpu_write_at_HL(cpu, cpu_read_data_after_opcode(cpu)));
            break;

        case LD_HLR_R8:
            M_REQUIRE_NO_ERR(cpu_write_at_HL(cpu, cpu_reg_get(cpu, extract_reg(lu->opcode, 0))));
            break;

        case LD_N16R_A:
            M_REQUIRE_NO_ERR(cpu_write_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_A_get(cpu)));
            break;
            
        case LD_N16R_SP:
            M_REQUIRE_NO_ERR(cpu_write16_at_idx(cpu, cpu_read_addr_after_opcode(cpu), cpu_reg_pair_SP_get(cpu, REG_AF_CODE)));
            break;

        case LD_N8R_A:
            M_REQUIRE_NO_ERR(cpu_write_at_idx(cpu, REGISTERS_START + cpu_read_data_after_opcode(cpu), cpu_A_get(cpu)));
            break;

        case LD_R16SP_N16:
            cpu_reg_pair_SP_set(cpu, extract_reg_pair(lu->opcode), cpu_read_addr_after_opcode(cpu));
            break;

        case LD_R8_HLR:
            cpu_reg_set(cpu, extract_reg(lu -> opcode, 3), cpu_read_at_HL(cpu));
            break;

        case LD_R8_N8:
            cpu_reg_set(cpu, extract_reg(lu->opcode,3), cpu_read_data_after_opcode(cpu));
            break;

        case LD_R8_R8: 
            cpu_reg_set(cpu, extract_reg(lu->opcode, 3), cpu_reg_get(cpu, extract_reg(lu->opcode, 0))); //TODO: need to verify that r != s?
            break;

        case LD_SP_HL:
            cpu_reg_pair_SP_set(cpu, REG_AF_CODE, cpu_HL_get(cpu));
            break;

        case POP_R16:
            cpu_reg_pair_set(cpu,  extract_reg_pair(lu -> opcode), cpu_SP_pop(cpu));
            break;

        case PUSH_R16:
            M_REQUIRE_NO_ERR(cpu_SP_push(cpu, cpu_reg_pair_get(cpu, extract_reg_pair(lu->opcode))));
            break;

        default:
            fprintf(stderr, "Unknown STORAGE instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
            return ERR_INSTR;
            break;
    } // switch

    return ERR_NONE;
}
