/**
 * @file bit.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @date 2020
 */

#include "cpu-registers.h"
#include "myMacros.h"
#include "bit.h"


// ==== see cpu-registers.h ========================================
uint8_t cpu_reg_get(const cpu_t* cpu, reg_kind reg){
    if(cpu == NULL) return 0;
    
    switch(reg){
        case REG_A_CODE: return cpu->A;
        case REG_B_CODE: return cpu->B;
        case REG_C_CODE: return cpu->C;
        case REG_D_CODE: return cpu->D;
        case REG_E_CODE: return cpu->E;
        case REG_H_CODE: return cpu->H;
        case REG_L_CODE: return cpu->L;
        default: return 0;
    }
}


// ==== see cpu-registers.h ========================================
void cpu_reg_set(cpu_t* cpu, reg_kind reg, uint8_t value){
    if(cpu == NULL) return;

    switch(reg){
        case REG_A_CODE: cpu->A = value; break;
        case REG_B_CODE: cpu->B = value; break;
        case REG_C_CODE: cpu->C = value; break;
        case REG_D_CODE: cpu->D = value; break;
        case REG_E_CODE: cpu->E = value; break;
        case REG_H_CODE: cpu->H = value; break;
        case REG_L_CODE: cpu->L = value; break;
        default: break;
        
        return;
    }

}



// ==== see cpu-registers.h ========================================
uint16_t cpu_reg_pair_get(const cpu_t* cpu, reg_pair_kind reg){
    if (cpu == NULL) return 0;

    switch(reg){
        case REG_BC_CODE: return cpu->BC;
        case REG_DE_CODE: return cpu->DE;
        case REG_HL_CODE: return cpu->HL;
        case REG_AF_CODE: return cpu->AF;
        default: return 0;
    }
}



// ==== see cpu-registers.h ========================================
void cpu_reg_pair_set(cpu_t* cpu, reg_pair_kind reg, uint16_t value){
    if(cpu == NULL) return;

    switch(reg){
        case REG_BC_CODE: cpu->BC = value; break;
        case REG_DE_CODE: cpu->DE = value; break;
        case REG_HL_CODE: cpu->HL = value; break;
        case REG_AF_CODE: cpu->AF = (value & 0xFFF0); break; 
        default: break;
        return;
    }
}

