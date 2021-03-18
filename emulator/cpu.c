/**
 * @file cpu.c
 * @brief Game Boy CPU simulation
 *
 * @date 2019
 */

#include "error.h"
#include "opcode.h"
#include "cpu.h"
#include "cpu-alu.h"
#include "cpu-registers.h"
#include "util.h"
#include "cpu-storage.h"
#include "myMacros.h"

#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf


/**
* @brief verify if a certain conditiion (cc) is verified or not
* @param  cpu cpu that stores the flags
* @param lu instruction to extract the condition
* @return the condition is verified or not
*/
int verify_cc(cpu_t* cpu, const instruction_t* lu);

/**
* @brief check if an interruption is pending if the same bit is 1 in IF and in IE
* @param cpu cpu to check
* @return content of REG_IF
*/
uint8_t pending_interruptions(cpu_t* cpu);


// ==== see cpu.h ========================================
int cpu_init(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu); 

    zero_init_ptr(cpu);
    M_REQUIRE_NO_ERR(component_create(&(cpu->high_ram), HIGH_RAM_SIZE));

    return ERR_NONE;
}

// ==== see cpu.h ========================================
int cpu_plug(cpu_t* cpu, bus_t* bus)
{
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(cpu);
    
    cpu->bus = bus; 
    M_REQUIRE_NO_ERR(bus_plug(*bus, &(cpu->high_ram), HIGH_RAM_START, HIGH_RAM_END));
    
    (*(cpu->bus))[REG_IE] = &(cpu->IE);
    (*(cpu->bus))[REG_IF] = &(cpu->IF);

    return ERR_NONE;
}

// ==== see cpu.h =======================================
void cpu_free(cpu_t* cpu)
{
    if(cpu == NULL) return;
    
    if(cpu->bus == NULL) {
        component_free(&(cpu->high_ram));
        return;
    }
    
    (*(cpu->bus))[REG_IE] = NULL;
    (*(cpu->bus))[REG_IF] = NULL;
    
    //freeing the high_ram
    
    bus_unplug(*(cpu->bus), &(cpu->high_ram));
    component_free(&(cpu->high_ram));

    cpu->bus = NULL;
    return;
}

/**
 * @brief Executes an instruction
 * @param lu instruction
 * @param cpu, the CPU which shall execute
 * @return error code
 *
 * See opcode.h and cpu.h
 */
static int cpu_dispatch(const instruction_t* lu, cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(lu);
    M_REQUIRE_NON_NULL(cpu);   


    cpu->idle_time += lu->cycles - 1;
    cpu->alu.value = 0; 
    cpu->alu.flags = 0;

    switch (lu->family) {

    // ALU
    case ADD_A_HLR: 
    case ADD_A_N8:
    case ADD_A_R8:
    case INC_HLR:
    case INC_R8:
    case ADD_HL_R16SP:
    case INC_R16SP:
    case SUB_A_HLR:
    case SUB_A_N8:
    case SUB_A_R8:
    case DEC_HLR:
    case DEC_R8:
    case DEC_R16SP:
    case AND_A_HLR:
    case AND_A_N8:
    case AND_A_R8:
    case OR_A_HLR:
    case OR_A_N8:
    case OR_A_R8:
    case XOR_A_HLR:
    case XOR_A_N8:
    case XOR_A_R8:
    case CPL:
    case CP_A_HLR:
    case CP_A_N8:
    case CP_A_R8:
    case SLA_HLR:
    case SLA_R8:
    case SRA_HLR:
    case SRA_R8:
    case SRL_HLR:
    case SRL_R8:
    case ROTCA:
    case ROTA:
    case ROTC_HLR:
    case ROT_HLR:
    case ROTC_R8:
    case ROT_R8:
    case SWAP_HLR:
    case SWAP_R8:
    case BIT_U3_HLR:
    case BIT_U3_R8:
    case CHG_U3_HLR:
    case CHG_U3_R8:
    case LD_HLSP_S8:
    case DAA:
    case SCCF:
        M_EXIT_IF_ERR(cpu_dispatch_alu(lu, cpu));
        cpu->PC += lu->bytes;
        break;

    // STORAGE
    case LD_A_BCR:
    case LD_A_CR:
    case LD_A_DER:
    case LD_A_HLRU:
    case LD_A_N16R:
    case LD_A_N8R:
    case LD_BCR_A:
    case LD_CR_A:
    case LD_DER_A:
    case LD_HLRU_A:
    case LD_HLR_N8:
    case LD_HLR_R8:
    case LD_N16R_A:
    case LD_N16R_SP:
    case LD_N8R_A:
    case LD_R16SP_N16:
    case LD_R8_HLR:
    case LD_R8_N8:
    case LD_R8_R8:
    case LD_SP_HL:
    case POP_R16:
    case PUSH_R16:
        M_EXIT_IF_ERR(cpu_dispatch_storage(lu, cpu));
        cpu->PC += lu->bytes;
        break;

    
    // JUMP
    case JP_CC_N16:
        if(verify_cc(cpu, lu)){
            cpu->PC = cpu_read_addr_after_opcode(cpu);
            cpu->idle_time += lu->xtra_cycles;

        } else{
            cpu->PC += lu->bytes; 
        }
        break;
    

    case JP_HL:
        cpu->PC = cpu_HL_get(cpu);
        break;

    case JP_N16:
        cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;

    case JR_CC_E8:
        control_pc_and(cpu, lu, cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu));
        break;

    case JR_E8:
        cpu->PC += lu->bytes + (int8_t) cpu_read_data_after_opcode(cpu);
        break;


    // CALLS
    case CALL_CC_N16:
        control_pc_and(cpu, lu, 
            cpu_SP_push(cpu, cpu->PC +lu->bytes);  
            cpu->PC = cpu_read_addr_after_opcode(cpu)
        );
        break;

    case CALL_N16:
        cpu_SP_push(cpu, cpu->PC + lu->bytes);
        cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;

    // RETURN (from call)
    case RET:
        cpu->PC = cpu_SP_pop(cpu);
        break;

    case RET_CC:
        control_pc_and(cpu, lu, cpu->PC = cpu_SP_pop(cpu));
        break;

    case RST_U3:
        cpu_SP_push(cpu, cpu->PC + lu->bytes);
        cpu->PC = (extract_n3(lu->opcode) << 3);
        break;


    // INTERRUPT & MISC.
    case EDI:
        cpu->IME = extract_ime(lu->opcode);
        cpu->PC += lu->bytes;
        break;

    case RETI:
        bit_set(&(cpu->IME), 0);
        cpu->PC = cpu_SP_pop(cpu);
        break;

    case HALT:
        cpu->HALT = 1;
        cpu->PC += lu->bytes;
        break;

    case STOP:
    case NOP:
        // ne rien faire
        cpu->PC += lu->bytes;
        break;


    default: {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    } break;

    } // switch

    return ERR_NONE;
}

/**
 * @brief Performs a cpu cycle
 * @param cpu, the CPU which shall execute
 * @return error code
 *
 * See cpu.h
 */
static int cpu_do_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    uint8_t pending = pending_interruptions(cpu);

    //if there are pending interruptions
    if(cpu->IME != 0 && pending != 0){
        
        //set IME to zero
        cpu->IME = 0;

        //get the index of rightmost set bit of pending interruptions
        interrupt_t ir = VBLANK;

        while(bit_get(pending, ir) != 1){
            ++ir;
        }

        //unsetting the corresponding bit in IF
        bit_unset(&(cpu->IF), ir);

        //push current PC address
        cpu_SP_push(cpu, cpu->PC);

        //setting the address of instruction handler in PC
        cpu->PC = ir_address(ir);
    
        //adding 5 cycles to idle_time
        cpu->idle_time += HANDLING_INTERRUPT;

        cpu->IME = 1;
        return ERR_NONE;
    } else {
        data_t bin = cpu_read_at_idx(cpu, cpu->PC);
        
        const instruction_t* instr;

        if(bin == PREFIXED){
            bin =  cpu_read_data_after_opcode(cpu);
            instr = &instruction_prefixed[bin];
        } else {
            instr = &instruction_direct[bin];
        }
        
        return cpu_dispatch(instr, cpu);
    }
}


// ==== see cpu.h ========================================
int cpu_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);
    
    cpu->write_listener = 0;

    if((cpu->HALT == 1 && pending_interruptions(cpu) != 0 && cpu->idle_time == 0) || (cpu->HALT == 0 && cpu->idle_time == 0)){
        cpu->HALT = 0;
        return cpu_do_cycle(cpu);
    } 

    if(cpu->idle_time > 0){
        cpu->idle_time -=1;
    }
    return ERR_NONE;
}

/**
 * @brief Set an interruption
 */
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i){
    bit_set(&(cpu->IF), i);
}


// ==== Tool method ========================================
int verify_cc(cpu_t* cpu, const instruction_t* lu){
    uint8_t cc = extract_cc(lu->opcode);
    flag_bit_t flag = bit_get(cc, 1) == 1 ? get_C(cpu->F) : get_Z(cpu->F);

    return bit_get(cc, 0) == 1 ? (flag != 0) : (flag == 0);
}

// ==== Tool method ========================================
uint8_t pending_interruptions(cpu_t* cpu){
    return (cpu_read_at_idx(cpu, REG_IF) & cpu_read_at_idx(cpu, REG_IE));
}
