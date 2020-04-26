/**
 * @file bit.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdlib.h>
#include "bit.h"

// ==== see bit.h ========================================
uint8_t lsb4(uint8_t value){
    return (value & 0xf);
}


// ==== see bit.h ========================================
uint8_t msb4(uint8_t value){
    return ((value & 0xf0) >> 4);
}


// ==== see bit.h ========================================
void bit_rotate(uint8_t* value, rot_dir_t dir, int d){
    if(value == NULL) return;
    
    d = CLAMP07(d); 
    
    int rest = (dir == LEFT) ? (*value >> (8 - d)) : (*value << (8 - d));
    *value = (dir == LEFT) ? (*value << d) : (*value >> d);
    
    *value |= rest;
    return;
}


// ==== see bit.h ========================================
uint8_t lsb8(uint16_t value){
    return (value & 0xff);
}


// ==== see bit.h ========================================
uint8_t msb8(uint16_t value){
    return ((value & 0xff00) >> 8);
}


// ==== see bit.h ========================================
uint16_t merge8(uint8_t v1, uint8_t v2){
    return ((v2 << 8) | v1);
}


// ==== see bit.h ========================================
uint8_t merge4(uint8_t v1, uint8_t v2){
    return ((lsb4(v2) << 4) | lsb4(v1));
}


// ==== see bit.h ========================================
bit_t bit_get(uint8_t value, int index){
    index = CLAMP07(index);
    return ((value & (1 << index)) >> index);
}


// ==== see bit.h ========================================
void bit_set(uint8_t* value, int index){
    if(value == NULL) return;
    *value = (*value | (1 << CLAMP07(index)));
    return;
}


// ==== see bit.h ========================================
void bit_unset(uint8_t* value, int index){
    if(value == NULL) return;
    *value = (*value & ~(1 << CLAMP07(index)));
    return;
}


// ==== see bit.h ========================================
void bit_edit(uint8_t* value, int index, uint8_t v){
    if(value == NULL) return;
    v != 0 ? bit_set(value, index) : bit_unset(value, index);
    return;
}