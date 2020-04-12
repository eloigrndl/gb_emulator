/**
 * @file alu.c
 * @brief ALU Body for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "alu.h"
#include "error.h"


/**
 * @brief get flag value
 *
 * @param flags flag set to get flag from
 * @param flag flag value to get
 * @return flag value
 */
bit_t get_flag(flags_t flags, flag_bit_t flag){
    switch(flag){                                                                      // returns bit value of the demanded flag, shifted to the right format
        case FLAG_Z: return bit_get(flags, 7) << 7;
        case FLAG_N: return bit_get(flags, 6) << 6;
        case FLAG_H: return bit_get(flags, 5) << 5;
        case FLAG_C: return bit_get(flags, 4) << 4;
        default: return ERR_NONE;
    }
}


/**
 * @brief set flag
 *
 * @param flags (modified) set of flags
 * @param flag flag to be set
 */
void set_flag(flags_t* flags, flag_bit_t flag){
    switch(flag){                                                                     // sets the bit of the demanded flag
        case FLAG_Z: bit_set(flags, 7);
                     break;
        case FLAG_N: bit_set(flags, 6);
                     break;
        case FLAG_H: bit_set(flags, 5);
                     break;
        case FLAG_C: bit_set(flags, 4);
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
    if(result == NULL)                                  // cannot work with a NULL pointer
        return ERR_BAD_PARAMETER;

    result->flags = 0; //resets all existing flags

    uint8_t lsb = lsb4(x) + lsb4(y) + c0;               // adds the 4 lsb of x and y, including the carry
    if(msb4(lsb) != 0)                                  // checks whether there has been an overflow on the 4-bitton addition, sets flag accordingly
        set_H(&result->flags); 

    uint8_t msb = msb4(x) + msb4(y) + msb4(lsb);        // adds the 4 msb of x and y, including the half-carry of the prior 4-bit addition
    if(msb4(msb) != 0)                                  // checks whether there has been an overflow on the 4-bitton addition, sets flag accordingly
        set_C(&result->flags);


    uint16_t total = merge4(lsb, msb);                  // merges the computed msb and lsb of our sum to obtain the desired result
    if(total == 0)                                      // checks whether out result is 0, sets flag accordingly
        set_Z(&result->flags);
    
    result->value = total;                              // assigns computed sum to result

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
    if(result == NULL)                                                  // cannot work with a NULL pointer
        return ERR_BAD_PARAMETER;

    result->flags = 0;                                                  // resets the flags 
    set_N(&result->flags);                                              // N flag is always set for this operation

    uint8_t lsb = lsb4(x) - lsb4(y) - b0;                               // performs a 4bit subtraction on x and y, including the initial borrow b0
    if(msb4(lsb) != 0)                                                  // checks whether there has been an underflow
        set_H(&result->flags);

    uint8_t msb = msb4(x) - msb4(y) + msb4(lsb);                        // performs a 4bit subtraction on x an y, including the halfborrows from the prior addition       
    if((y + b0) > x)                                                    // an underflow will always occur when the minuend is smaller than the sum of the substrahend and the inital borrow
        set_C(&result->flags);
    
    uint16_t total = merge4(lsb, msb);                                  // merges the msb and lsb of the overall difference
    if(total == 0)                                                      // checks whether difference is 0, sets flag accordingly
        set_Z(&result->flags);

    result->value = total;

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
    if(result == NULL)                                                  // cannot work with Nullpointer
        return ERR_BAD_PARAMETER;

    error_code e = alu_add8(result, lsb8(x), lsb8(y), 0);               // performs 8bit addition on the lsb of x and y, stores error code
    if(e != ERR_NONE)                                                   // forwards error of prior addition
        return e;         

    flags_t lowFlags = result->flags;                                   // stores the result and resulted flags of the lsb addition
    uint8_t lsb = result->value;


    e = alu_add8(result, msb8(x), msb8(y), bit_get(lowFlags, 4));       // FIXME: magic nr performs 8bit addition on the msb of x and y, as well as any carry, stores error code 
    if(e != ERR_NONE)                                                   // forwards error of prior addition
        return e; 

    result->value = merge8(lsb, result->value);                         // merges lsb and msb of the two additions

    result->flags = 0;                                                  // resets all of the flags
    if(get_flag(lowFlags, FLAG_C) != 0)                                 // adapts flag C of lsb addition
        set_C(&result->flags); 

    if(get_flag(lowFlags, FLAG_H) != 0)                                 // adapts flag H of lsb addition
        set_H(&result->flags);

    if(result->value == 0)                                              // checks whether total is zero, sets flag accordingly
        set_Z(&result->flags);
    
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
        if(result == NULL)                                              // cannot work with Nullpointer
        return ERR_BAD_PARAMETER;

    error_code e = alu_add8(result, lsb8(x), lsb8(y), 0);               // performs 8bit addition on the lsb of x and y, stores error code
    if(e != ERR_NONE)                                                   // forwards error of prior addition
        return e;         

    uint8_t lsb = result->value;


    e = alu_add8(result, msb8(x), msb8(y), bit_get(result->flags, 4));  // FIXME: magic nr performs 8bit addition on the msb of x and y, as well as any carry, stores error code 
    if(e != ERR_NONE)                                                   // forwards error of prior addition
        return e; 

    flags_t hiFlags = result->flags;                                    // stores the flag of the msb addition
    result->value = merge8(lsb, result->value);                         // merges lsb and msb of the two additions

    result->flags = 0;                                                  // resets all of the flags
    if(get_flag(hiFlags, FLAG_C) != 0)                                  // adapts flag C of msb addition
        set_C(&result->flags); 

    if(get_flag(hiFlags, FLAG_H) != 0)                                  // adapts flag H of msb addition
        set_H(&result->flags);

    if(result->value == 0)                                              // checks whether total is zero, sets flag accordingly
        set_Z(&result->flags);


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
    if(result == NULL || dir < LEFT || dir > RIGHT)                     // Cannot work with Nullpointer, or direction which is out of bounds
        return ERR_BAD_PARAMETER;

    bit_t left = 0;                                                     // will store bit lost by shift

    if(dir == LEFT){
        left = bit_get(x, 7);                                           // in case of shift-left, will lose bit 7
        x <<= 1;                                                        // shifts x to the left by 1 bit
    }else{  
        left = bit_get(x, 0);                                           // in case of shift-rgith, will lose bit 0
        x >>= 1;                                                        // shift x to the right by 1 bit
    }

    result->value = x;                                                  // sets the results value to the shifted x
    result->flags = 0;                                                  // resets all flags

    if(x == 0)                                                          // checks whether result after shift is 0, sets Z flag accordingly
        set_Z(&result->flags);

    if(left == 1)                                                       // checks whether lost bit was a 1, sets C flag accordingly
        set_C(&result->flags);

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
    if(result == NULL)                                      // cannot work with Nullpointer
        return ERR_BAD_PARAMETER;

    bit_t msb = bit_get(x, 7);                              // stores msb before performing shift
    bit_t left = bit_get(x, 0);                             // stores msb that will be lost after shift

    x >>= 1,                                                // shifts x by 1 to the right
    bit_edit(&x, 7, msb);                                   // replaces 0 of shifted x by the previously saves msb (arithmetic shift)

    result->value = x;
    result->flags = 0;

    if(x == 0)                                              // checks whether result after shift is 0, sets Z flag accordingly
        set_Z(&result->flags);


    if(left == 1)                                           // checks whether lost bit was a 1, sets C flag accordingly
        set_C(&result->flags);

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
    if(result == NULL || dir < LEFT || dir > RIGHT)                     // Cannot work with Nullpointer, or direction which is out of bounds
        return ERR_BAD_PARAMETER;


    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);           // stores bit that will "wrap" based on direction
    bit_rotate(&x, dir, 1);                                             // rotates bit by 1 in given direction

    result->value = x;
    result->flags = 0;

    if(x == 0)                                                          // checks whether result after rotation is 0, sets Z flag accordingly
        set_Z(&result->flags);

    if(left == 1)                                                      // checks whether wrapped bit was a 1, sets C flag accordingly
        set_C(&result->flags);

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
    if(result == NULL || dir < LEFT || dir > RIGHT)
        return ERR_BAD_PARAMETER;

    bit_t left = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);                          // bit to be wrapped around depends on direction
    
    bit_t carry = bit_get(flags, 4);
    alu_shift(result, x, dir);                                                         // performs the shift itself

    if(dir == RIGHT)                                                                    // format of carry to be added depends on direction, the carry itself is defined by the added flag
        carry <<= 7;

    result->value |= carry;                                                            // FIXME: avoid copy paste by adding another method? // Appends the previously saved carry
    result->flags = 0;

    if(result->value == 0)                                                             // checks whether result after rotation is 0, sets Z flag accordingly
        set_Z(&result->flags);

    if(left != 0)                                                                      // checks whether wrapped bit was a 1, sets C flag accordingly
        set_C(&result->flags);
    printf("X: %d, Carry: %d resullt: %d\n", x, carry, result->value);
    return ERR_NONE;
}
