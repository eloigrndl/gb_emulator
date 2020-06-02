/**
 * @file alu.c
 * @brief ALU Body for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "alu.h"
#include "error.h"


//TODO remove some of these indices
//================== helpers defined by us ======================
/**
 * @brief indices for the bits of the various flag values
 */
#define INDEX_FLAG_Z 7
#define INDEX_FLAG_N 6
#define INDEX_FLAG_H 5
#define INDEX_FLAG_C 4

/**
 * @brief resets the flags
 */
 #define reset_flags(result) \
    result->flags = 0;

 /**
 * @brief checks whether or not the result is null : if so, sets the Z-flag
 */   
#define edit_zero_flag(result) \
    set_flag_if(FLAG_Z, result->value == 0, result);

/**
 * @brief sets a flag if the corresponding condition is true
 * 
 * @param flags flag set to set the flag
 * @param flag the flag that should be set
 * @param condition condition on which the flag is set
 * @param result result on which the condition is evaluated 
 */
void set_flag_if(flag_bit_t flag, int condition, alu_output_t* result){
    if(result == NULL) return;

    if(condition)
        set_flag(&result->flags, flag);
    return;
}

/**
 * @brief Sets the flag accordingly after a shift operation
 * @param result The resulte whose flags should be set
 * @param left the bit used for the carry operation
 */
void set_flags_after_shift(alu_output_t* result, bit_t left){
    if(result == NULL) return;

    reset_flags(result);
    edit_zero_flag(result);
    set_flag_if(FLAG_C, left != 0, result);
    return;
}

//================== end of helpers ======================



// ==== see alu.h ========================================
flag_bit_t get_flag(flags_t flags, flag_bit_t flag){
    M_REQUIRE((flag == FLAG_Z || flag == FLAG_N || flag == FLAG_H || flag == FLAG_C), 
    ERR_BAD_PARAMETER, "flag %d is not valid", flag);   //TODO; is this right?MY

    return flags & flag;
}


// ==== see alu.h ========================================
void set_flag(flags_t* flags, flag_bit_t flag){
    if(flags == NULL || ((flag != FLAG_Z && flag != FLAG_N && flag != FLAG_H && flag != FLAG_C) == 1)) return;   

    *flags = *flags | flag;
    return;
}


// ==== see alu.h ========================================
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){
    M_REQUIRE_NON_NULL(result);

    reset_flags(result);

    uint8_t lsb = lsb4(x) + lsb4(y) + c0;
    set_flag_if(FLAG_H, msb4(lsb) != 0, result);

    uint8_t msb = msb4(x) + msb4(y) + msb4(lsb);
    set_flag_if(FLAG_C, msb4(msb) != 0, result);

    uint16_t total = merge4(lsb, msb);
    result->value = total;
    edit_zero_flag(result);

    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
    M_REQUIRE_NON_NULL(result);

    reset_flags(result);
    set_N(&result->flags);

    uint8_t lsb = lsb4(x) - lsb4(y) - b0;
    set_flag_if(FLAG_H, msb4(lsb) != 0, result);

    uint8_t msb = msb4(x) - msb4(y) + msb4(lsb);     
    set_flag_if(FLAG_C, (y + b0) > x, result);
    
    uint16_t total = merge4(lsb, msb);
    result->value = total;
    edit_zero_flag(result);
       
    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y){
    M_REQUIRE_NON_NULL(result);

    M_REQUIRE_NO_ERR(alu_add8(result, lsb8(x), lsb8(y), 0));
    flags_t lowFlags = result->flags;                                                       
    uint8_t lsb = result->value;


    M_REQUIRE_NO_ERR(alu_add8(result, msb8(x), msb8(y), get_C(lowFlags) >> INDEX_FLAG_C)); 
    result->value = merge8(lsb, result->value);
    reset_flags(result);                                                                          
    set_flag_if(FLAG_C, get_flag(lowFlags, FLAG_C) != 0, result);                                
    set_flag_if(FLAG_H, get_flag(lowFlags, FLAG_H) != 0, result);                           
        
    edit_zero_flag(result);
    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y){
    M_REQUIRE_NON_NULL(result);

    M_REQUIRE_NO_ERR(alu_add8(result, lsb8(x), lsb8(y), 0));
    uint8_t lsb = result->value;


    M_REQUIRE_NO_ERR(alu_add8(result, msb8(x), msb8(y), bit_get(result->flags, INDEX_FLAG_C)));
    flags_t hiFlags = result->flags;
    result->value = merge8(lsb, result->value);

    reset_flags(result);
    set_flag_if(FLAG_C, get_flag(hiFlags, FLAG_C) != 0, result);
    set_flag_if(FLAG_H, get_flag(hiFlags, FLAG_H) != 0, result);
    edit_zero_flag(result);
    
    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = (dir == LEFT) ? bit_get(x, 7) : bit_get(x, 0);
    x = (dir == LEFT) ? x << 1 : x >> 1;
    result->value = x;

    set_flags_after_shift(result, left);
    
    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_shiftR_A(alu_output_t* result, uint8_t x){
    M_REQUIRE_NON_NULL(result);

    bit_t msb = bit_get(x, 7);
    bit_t left = bit_get(x, 0);

    result->value = x >> 1 | (msb << 7);

    set_flags_after_shift(result, left);

    return ERR_NONE;
}


// ==== see alu.h ========================================
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);
    bit_rotate(&x, dir, 1);
    
    result->value = x;
    set_flags_after_shift(result, left);

    return ERR_NONE;

}


// ==== see alu.h ========================================
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);
    bit_t carry = dir == LEFT ? (get_C(flags) >> INDEX_FLAG_C) : (get_C(flags) >> INDEX_FLAG_C) << 7;

    alu_shift(result, x, dir);
    result->value |= carry;

    set_flags_after_shift(result, left);

    return ERR_NONE;
}
    




