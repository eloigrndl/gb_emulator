//TODO COMMENT!!!


#include "bit.h"


uint8_t lsb4(uint8_t value){
    return (value & 0xf);
}

uint8_t msb4(uint8_t value){
    return ((value & 0xf0) >> 4);
}

uint8_t lsb8(uint16_t value){
    return (value & 0xff);
}
 
uint8_t msb8(uint16_t value){
    return ((value & 0xff00) >> 8);
}

uint16_t merge8(uint8_t v1, uint8_t v2){
    return ((lsb8(v2) << 8) | lsb8(v1));
}

uint8_t merge4(uint8_t v1, uint8_t v2){
    return ((lsb4(v2) << 4) | lsb4(v1));
}

bit_t bit_get(uint8_t value, int index){
    index = CLAMP07(index);
    return ((value & (1 << index)) >> index);
}

void bit_set(uint8_t* value, int index){
    uint8_t mask = 1 << CLAMP07(index);
    uint8_t result = *value | mask;
    *value = result;
    return;
}

void bit_unset(uint8_t* value, int index){
    uint8_t mask = 1 << CLAMP07(index);
    uint8_t result = *value & ~mask;
    *value = result;
    return;
}

void bit_rotate(uint8_t* value, rot_dir_t dir, int d){
    d = CLAMP07(d);
    int rest = 0;

    if(dir == LEFT){
        rest = *value >> (8 - d);
        *value = *value << d;
    }else{
        rest = *value << (8 - d);
        *value = *value >> d;
    }

    *value |= rest;
    return;
}

void bit_edit(uint8_t* value, int index, uint8_t v){
    if(v)
        bit_set(value, index);
    else
        bit_unset(value, index);
    return;
}