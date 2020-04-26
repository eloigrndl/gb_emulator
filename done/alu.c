/**
 * @file alu.c
 * @brief ALU Body for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

//TODO add bit_edit to all methods

#include "alu.h"
#include "error.h"


/**
 * @brief get flag value
 *
 * @param flags flag set to get flag from
 * @param flag flag value to get
 * @return flag value
 */
flag_bit_t get_flag(flags_t flags, flag_bit_t flag){
    switch(flag){                                                                      // returns bit value of the demanded flag, shifted to the right format
        case FLAG_Z: return bit_get(flags, INDEX_FLAG_Z) << INDEX_FLAG_Z;                                   
        case FLAG_N: return bit_get(flags, INDEX_FLAG_N) << INDEX_FLAG_N;
        case FLAG_H: return bit_get(flags, INDEX_FLAG_H) << INDEX_FLAG_H;
        case FLAG_C: return bit_get(flags, INDEX_FLAG_C) << INDEX_FLAG_C;
        default: return 0;
    }
}


/**
 * @brief set flag
 *
 * @param flags (modified) set of flags
 * @param flag flag to be set
 */
void set_flag(flags_t* flags, flag_bit_t flag){
    if(flags == NULL) return;

    switch(flag){                                                                     // sets the bit of the demanded flag
        case FLAG_Z: bit_set(flags, INDEX_FLAG_Z);
                     break;
        case FLAG_N: bit_set(flags, INDEX_FLAG_N);
                     break;
        case FLAG_H: bit_set(flags, INDEX_FLAG_H);
                     break;
        case FLAG_C: bit_set(flags, INDEX_FLAG_C);
                     break;
        default: return;
    }
}


/**
 * @brief adds two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @param c0 carry in
 * @return error code
 */
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){
    M_REQUIRE_NON_NULL(result);

    reset_flags(result); //resets all existing flags

    uint8_t lsb = lsb4(x) + lsb4(y) + c0;               // adds the 4 lsb of x and y, including the carry
    set_flag_if(FLAG_H, msb4(lsb) != 0, result);        // checks whether there has been an overflow on the 4-bitton addition, sets flag accordingly

    uint8_t msb = msb4(x) + msb4(y) + msb4(lsb);        // adds the 4 msb of x and y, including the half-carry of the prior 4-bit addition
    set_flag_if(FLAG_C, msb4(msb) != 0, result);        // checks whether there has been an overflow on the 4-bitton addition, sets flag accordingly

    uint16_t total = merge4(lsb, msb);                  // merges the computed msb and lsb of our sum to obtain the desired result
    result->value = total;                              // assigns computed sum to result
    set_zero_flag(result);                              // checks whether out result is 0, sets flag accordingly
    
                           
    return ERR_NONE;                                   
}


/**
 * @brief subtract two uint8 and writes the results and flags into an alu_output_t structure
 *
 * @param result alu_output_t pointer to write into
 * @param x value to subtract from
 * @param y value to subtract
 * @param b0 initial borrow bit
 * @return error code
 */
int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
    M_REQUIRE_NON_NULL(result);

    reset_flags(result);                                                // resets the flags 
    set_N(&result->flags);                                              // N flag is always set for this operation

    uint8_t lsb = lsb4(x) - lsb4(y) - b0;                               // performs a 4bit subtraction on x and y, including the initial borrow b0
    set_flag_if(FLAG_H, msb4(lsb) != 0, result);                        // checks whether there has been an underflow

    uint8_t msb = msb4(x) - msb4(y) + msb4(lsb);                        // performs a 4bit subtraction on x an y, including the halfborrows from the prior addition       
    set_flag_if(FLAG_C, (y + b0) > x, result);                          // an underflow will always occur when the minuend is smaller than the sum of the substrahend and the inital borrow
    
    uint16_t total = merge4(lsb, msb);                                  // merges the msb and lsb of the overall difference
    result->value = total;                                              // assigns computed sum to result
    set_zero_flag(result);                                              // checks whether out result is 0, sets flag accordingly
       
    return ERR_NONE;
}


/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 lsb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y){
    M_REQUIRE_NON_NULL(result);

    M_REQUIRE_NO_ERR(alu_add8(result, lsb8(x), lsb8(y), 0));                      // performs 8bit addition on the lsb of x and y, stores error code
    flags_t lowFlags = result->flags;                                             // stores the result and resulted flags of the lsb addition
    uint8_t lsb = result->value;


    M_REQUIRE_NO_ERR(alu_add8(result, msb8(x), msb8(y), get_C(lowFlags) >> INDEX_FLAG_C)); //performs 8bit addition on the msb of x and y, as well as any carry, stores error code 
    result->value = merge8(lsb, result->value);                                   // merges lsb and msb of the two additions
   
    reset_flags(result);                                                  // resets all of the flags
    set_flag_if(FLAG_C, get_flag(lowFlags, FLAG_C) != 0, result);                                 // adapts flag C of lsb addition
    set_flag_if(FLAG_H, get_flag(lowFlags, FLAG_H) != 0, result);                                 // adapts flag H of lsb addition
        
    set_zero_flag(result);
    return ERR_NONE;
}


/**
 * @brief sum two uint16 and writes the results and flags into an alu_output_t structure,
 *        the H & C flags are being placed according to the 8 msb
 *
 * @param result alu_output_t pointer to write into
 * @param x value to sum
 * @param y value to sum
 * @return error code
 */
int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y){
    M_REQUIRE_NON_NULL(result);

    M_REQUIRE_NO_ERR(alu_add8(result, lsb8(x), lsb8(y), 0));               // performs 8bit addition on the lsb of x and y, stores error code
    uint8_t lsb = result->value;


    M_REQUIRE_NO_ERR(alu_add8(result, msb8(x), msb8(y), bit_get(result->flags, INDEX_FLAG_C)));  //performs 8bit addition on the msb of x and y, as well as any carry, stores error code 
    flags_t hiFlags = result->flags;                                                  // stores the flag of the msb addition
    result->value = merge8(lsb, result->value);                                       // merges lsb and msb of the two additions

    reset_flags(result);                                                              // resets all of the flags
    set_flag_if(FLAG_C, get_flag(hiFlags, FLAG_C) != 0, result);                      // adapts flag C of msb addition
    set_flag_if(FLAG_H, get_flag(hiFlags, FLAG_H) != 0, result);                      // adapts flag H of msb addition
    set_zero_flag(result);                                                            // checks whether total is zero, sets flag accordingly
    
    return ERR_NONE;
}


/**
 * @brief logic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @param dir shift direction
 * @return error code
 */
int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = (dir == LEFT) ? bit_get(x, 7) : bit_get(x, 0);                          // bit to be wrapped around depends on direction
    x = (dir == LEFT) ? x << 1 : x >> 1;
    result->value = x;

    set_flags_after_shift(result, left);
    
    return ERR_NONE;
}


/**
 * @brief arithmetic shift
 *
 * @param result alu_output_t pointer to write into
 * @param x value to shift
 * @return error code
 */
int alu_shiftR_A(alu_output_t* result, uint8_t x){
    M_REQUIRE_NON_NULL(result);

    bit_t msb = bit_get(x, 7);                              // stores msb before performing shift
    bit_t left = bit_get(x, 0);                             // stores msb that will be lost after shift

                                                            // replaces 0 of shifted x by the previously saves msb (arithmetic shift)
    result->value = x >> 1 | (msb << 7);

    set_flags_after_shift(result, left);

    return ERR_NONE;
}


/**
 * @brief logic rotate
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @return error code
 */
int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);           // stores bit that will "wrap" based on direction
    bit_rotate(&x, dir, 1);                                             // rotates bit by 1 in given direction
    
    result->value = x;
    set_flags_after_shift(result, left);

    return ERR_NONE;

}


/**
 * @brief logic rotate with carry taken into account
 *
 * @param result alu_output_t pointer to write into
 * @param x value to rotate
 * @param dir rotation direction
 * @param flags carry flag
 * @return error code
 */
int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags){
    M_REQUIRE_NON_NULL(result);
    M_REQUIRE(dir == LEFT || dir == RIGHT, ERR_BAD_PARAMETER, "direction %d is not valid", dir);   

    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);                          // bit to be wrapped around depends on direction
    bit_t carry = dir == LEFT ? (get_C(flags) >> INDEX_FLAG_C) : (get_C(flags) >> INDEX_FLAG_C) << 7;

    alu_shift(result, x, dir);                                                         // performs the shift itself
    result->value |= carry;                                                            // use macro FIXME: avoid copy paste by adding macro for flags? // Appends the previously saved carry

    set_flags_after_shift(result, left);

    return ERR_NONE;
}
    
    
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
    set_zero_flag(result);
    set_flag_if(FLAG_C, left == 1, result);
    return;
}

