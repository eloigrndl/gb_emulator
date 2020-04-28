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

#include <inttypes.h> // PRIX8
#include <stdio.h> // fprintf

// ======================================================================
int cpu_init(cpu_t* cpu)
{
    if(cpu == NULL)
        return ERR_BAD_PARAMETER;


    //TODO: use memset or not??
    cpu_AF_set(cpu, 0);
    cpu_BC_set(cpu, 0);
    cpu_DE_set(cpu, 0);
    cpu_HL_set(cpu, 0);
    
    cpu->SP = 0;
    cpu->PC = 0;

    //FIXME ok ?
    cpu->IME = 0;
    cpu->HALT = 0;

    //TODO: alu init?
    cpu->alu.flags = 0;
    cpu->alu.value = 0;    
    cpu -> idle_time = 0;

    //TODO: why high ram in cpu and not directly in gameboy
    component_create(&(cpu->high_ram), HIGH_RAM_SIZE); 


    return ERR_NONE;
}

// ======================================================================
int cpu_plug(cpu_t* cpu, bus_t* bus)
{
    if(bus == NULL || cpu == NULL)
        return ERR_BAD_PARAMETER;

    cpu -> bus = bus;
    bus_plug(*(cpu->bus), &(cpu->high_ram), HIGH_RAM_START, HIGH_RAM_END);
    return ERR_NONE;
}

// ======================================================================
void cpu_free(cpu_t* cpu)
{
     //FIXME can't return error code in void method
    
    cpu->bus = NULL;

    //freeing the high_ram
    component_free(&(cpu->component));
    &(cpu->component) = NULL.
    return;
    
}

//=========================================================================
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
    M_REQUIRE_NON_NULL(cpu);    //TODO: make usage of this macro for args

    //FIXME sure about this ?
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
        break;


    // JUMP
    case JP_CC_N16:
        if(verify_cc(lu))
            cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;

    case JP_HL:
        cpu->PC = cpu_reg_pair_get(cpu, REG_HL_CODE);
        break;

    case JP_N16:
        cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;

    case JR_CC_E8:
        //FIXME is PC'= PC+1 ?? and will this be really signed value
        if(verify_cc(lu))
            cpu->PC = cpu_read_data_after_opcode(cpu); + 1;
        break;

    case JR_E8:
        cpu->PC = cpu_read_data_after_opcode(cpu) + 1;
        break;


    // CALLS
    case CALL_CC_N16:
        //FIXME PC' = PC+1 ??
        if(verify_cc(lu)){
            cpu_SP_push(cpu, cpu->PC + 1);
            cpu->PC = cpu_read_addr_after_opcode(cpu);
        }
        break;

    case CALL_N16:
        cpu_SP_push(cpu, cpu->PC + 1);
        cpu->PC = cpu_read_addr_after_opcode(cpu);
        break;


    // RETURN (from call)
    case RET:
        cpu->PC = cpu_SP_pop(cpu);
        break;

    case RET_CC:
        if(verify_cc(lu))
            cpu->PC = cpu_SP_pop(cpu);
        break;

    case RST_U3:
        //FIXME PC' = PC+1
        cpu_SP_push(cpu, cpu->PC+1);
        cpu->PC = exctract_n3(lu->opcode) << 3;
        break;


    // INTERRUPT & MISC.
    case EDI:
        //FIXME IME as reg in cpu ?
        cpu->IME = bit_get(lu->opcode, 3);
        break;

    case RETI:
        bit_set(&(cpu->IME), 3);
        cpu->PC = cpu_SP_pop(cpu);
        break;

    case HALT:
        break;

    case STOP:
    case NOP:
        // ne rien faire
        break;

    default: {
        fprintf(stderr, "Unknown instruction, Code: 0x%" PRIX8 "\n", cpu_read_at_idx(cpu, cpu->PC));
        return ERR_INSTR;
    } break;

    } // switch


    //TODO: update PC, etc.
    cpu->idle_time -= lu->cycles;
    cpu->PC += lu->bytes;

    return ERR_NONE;
}

// ----------------------------------------------------------------------
static int cpu_do_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    uint8_t pendings = is_pending(cpu);

    //if there are oendings interruption
    if(cpu->IME == 1 && pendings != 0){
        //set IME to zero
        cpu->IME = 0;

        //get the index of rightmost set bit of pending interruptions
        int index = 0;
        while(bit_get(pendings, 0) != 1){
            ++index;
            pendings >>= 1;
        }

        //unsetting the corresponding bit in IF
        data_t prev_interrupt = cpu_read_at_idx(cpu, REG_IF);
        bit_unset(&prev_interrupt, index);
        cpu_write_at_idx(cpu, REG_IF, prev_interrupt);

        //push current PC address
        cpu_SP_push(cpu, cpu->PC);

        //setting the address of instruction handler in PC
        cpu->PC = 0x40 + 8 × index;
    
        //adding 5 cycles to idle_time
        cpu->idle_time += 5;

        return ERR_NONE;
    } else {
        data_t bin = cpu_read_at_idx(cpu, cpu -> PC);
        const instruction_t* instr;

        if(bin == 0xCB){
            bin =  cpu_read_data_after_opcode(cpu);
            instr = &instruction_prefixed[bin];
        } else {
            instr = &instruction_direct[bin];
        }

        return cpu_dispatch(instr, cpu);
    }
}


// ======================================================================
/**
 * See cpu.h
 */
int cpu_cycle(cpu_t* cpu)
{
    M_REQUIRE_NON_NULL(cpu);
    M_REQUIRE_NON_NULL(cpu->bus);

    if(cpu->HALT == 1 && is_pending(cpu) != 0 && cpu->idle_time == 0){
        cpu->HALT = 0;
        return cpu_do_cycle(cpu);
    }

    if(cpu -> idle_time > 0){
        cpu -> idle_time -=1;
    }
    return ERR_NONE;
}

/**
 * @brief Set an interruption
 */
void cpu_request_interrupt(cpu_t* cpu, interrupt_t i){
    data_t prev_interrupt = cpu_read_at_idx(cpu, REG_IF);
    bit_set(&prev_interrupt, i);
    cpu_write_at_idx(cpu, REG_IF, prev_interrupt);
}




//FIXME add also in cpu.h
/**
* @brief verify if a certain conditiion (cc) is verified or not
* @param  lu instruction to extract the condition
* @return the condition is verified or not
*/
int verify_cc(const instruction_t* lu){
    switch(exctract_cc(lu->opcode)){
        case 0 :
                return !get_Z(cpu->alu.flags);

            case 1 :
                return get_Z(cpu->alu.flags);

            case 2 :
                return !get_C(cpu->alu.flags);

            case 3 :
                return get_C(cpu->alu.flags);

            //FIXME:really necessary
            default:
                return 0;
    }
}

//FIXME put also in cpu.h or define ??
/**
* @brief check if an interruption is pending ;:
         if the same bit is 1 in IF and in IE
*/
uint8_t is_pending(cpu_t* cpu){
    return (cpu_read_at_idx(cpu, REF_IF) & cpu_read_at_idx(cpu, REG_IE));
}
