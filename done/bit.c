/**
 * @file bit.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdlib.h>
#include "bit.h"

/**
 * @brief returns the 4 LSB of a uint8_t
 *
 * @param value value to get the 4 LSB from
 * @return 4 LSB of the input
 */
uint8_t lsb4(uint8_t value){
    return (value & 0xf);
}


/**
 * @brief returns the 4 MSB of a uint8_t
 *
 * @param value value to get the 4 MSB from
 * @return 4 MSB of the input
 */
uint8_t msb4(uint8_t value){
    return ((value & 0xf0) >> 4);
}


/**
 * @brief Rotates the bits in a uint8_t
 *
 * @param value pointer to the number to rotate
 * @param dir rotation direction
 * @param d number of rotation steps to execute
 */

void bit_rotate(uint8_t* value, rot_dir_t dir, int d){
    if(value == NULL) return;
    
    d = CLAMP07(d); 
    
    int rest = (dir == LEFT) ? (*value >> (8-d)) : (*value << (8 - d));
    *value = (dir == LEFT) ? (*value << d) : (*value >> d);
    
    *value |= rest;
    return;
}


/**
 * @brief returns the 8 LSB of a uint16_t
 *
 * @param value value to get the 8 LSB from
 * @return 8 LSB of the input
 */
uint8_t lsb8(uint16_t value){
    return (value & 0xff);
}


/**
 * @brief returns the 8 MSB of a bit16_t (uint16_t)
 *
 * @param value value to get the 8 MSB from
 * @return 8 MSB of the input
 */
uint8_t msb8(uint16_t value){
    return ((value & 0xff00) >> 8);
}

/**
 * @brief Merges two 8bit into 16bits
 *
 * @param v1 value for the 8 LSB
 * @param v2 value for the 8 MSB
 * @return result of merging v1 and v2 into 16bits
 */
uint16_t merge8(uint8_t v1, uint8_t v2){
    return ((v2 << 8) | v1);
}


/**
 * @brief Merges two 4bit into 8bits
 *
 * @param v1 value for the 4 LSB
 * @param v2 value for the 4 MSB
 * @return result of merging v1 and v2 into 8bits
 */
uint8_t merge4(uint8_t v1, uint8_t v2){
    return ((lsb4(v2) << 4) | lsb4(v1));
}


/**
 * @brief Gets the bit at a given index
 *
 * @param value value to get the bit from
 * @param index index of the bit
 * @return returns the bit at a given index
 */
bit_t bit_get(uint8_t value, int index){
    index = CLAMP07(index);
    return ((value & (1 << index)) >> index);
}


/**
 * @brief Set the bit at a given index to 1
 *
 * @param value value to change the bit in
 * @param index index of the bit
 */
void bit_set(uint8_t* value, int index){
    if(value == NULL) return;
    *value = (*value | (1 << CLAMP07(index)));
    return;
}


/**
 * @brief Set the bit at a given index to 0
 *
 * @param value value to change the bit in
 * @param index index of the bit
 */
void bit_unset(uint8_t* value, int index){
    if(value == NULL) return;
    *value = (*value & ~(1 << CLAMP07(index)));
    return;
}


/**
 * @brief Set/Unset the bit at a given index
 *
 * @param value value to change the bit in
 * @param index index of the bit
 * @param v allows to choose between set and unset
 */
void bit_edit(uint8_t* value, int index, uint8_t v){
    if(value == NULL) return;
    v != 0 ? bit_set(value, index) : bit_unset(value, index);
    return;
}