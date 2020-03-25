#include "alu.h"
#include "error.h"


//TODO BEWARE OF ARGUMENT VERIFICATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1!
//TODO INCLUDE ERRORCODES!!!!!
//TODO replace all  bit_edit with set_flag
int alu_add8(alu_output_t* result, uint8_t x, uint8_t y, bit_t c0){
    if(result == NULL)
        return ERR_BAD_PARAMETER;

    flags_t generatedFlags = 0;
    uint8_t lsb = lsb4(x) + lsb4(y) + c0;
    
    bit_edit(&(generatedFlags), 5,  bit_get(lsb, 4)); // TODO: magic numbers
    uint8_t msb = msb4(x) + msb4(y) + msb4(lsb);
    
    bit_edit(&(generatedFlags), 4,  bit_get(msb, 4));
    uint16_t total = merge4(lsb, msb);
    
    bit_edit(&(generatedFlags), 7, 0 == total);
    
    result->value = total;
    result->flags = generatedFlags;

    return ERR_NONE;               // TODO: edit error msg
}

int alu_sub8(alu_output_t* result, uint8_t x, uint8_t y, bit_t b0){
    if(result == NULL)
        return ERR_BAD_PARAMETER;

    flags_t generatedFlags = 0x40;

    uint8_t lsb = lsb4(x) - lsb4(y) - b0;
    bit_edit(&(generatedFlags), 5,  bit_get(lsb, 4) || b0);         // TODO: implementation of b0 needs to be CORRECTED
    
    uint8_t msb = msb4(x) - msb4(y) + msb4(lsb);                    
    bit_edit(&(generatedFlags), 4,  bit_get(msb, 4) || b0);
    
    uint16_t total = merge4(lsb, msb);
    bit_edit(&(generatedFlags), 7, 0 == total);

    (*result).value = total;
    result->flags = generatedFlags;

    return ERR_NONE;
}

int alu_add16_low(alu_output_t* result, uint16_t x, uint16_t y){
    if(result == NULL)
        return ERR_BAD_PARAMETER;

    int i = alu_add8(result, lsb8(x), lsb8(y), 0);
    flags_t halfFlags = result->flags;
    uint8_t lsb = result->value;

    if(i != 0)
        return i;           //TODO: WHY? NEED TO COMMENT CODE 

    i = alu_add8(result, msb8(x), msb8(y), bit_get(result->flags, 4));
    if(i != 0)
        return i;
    
    result->value = merge8(lsb, result->value);
    bit_edit(&(halfFlags), 7, 0 == result->value);
    bit_edit(&(halfFlags), 6, 0);
    result->flags = halfFlags;
    
    return ERR_NONE;
}

int alu_add16_high(alu_output_t* result, uint16_t x, uint16_t y){
    if(result == NULL)
        return ERR_BAD_PARAMETER;
    
    int i = alu_add8(result, lsb8(x), lsb8(y), 0);
    uint8_t lsb = result->value;
    if(i != 0)
        return i;
    i = alu_add8(result, msb8(x), msb8(y), bit_get(result->flags, 4));
    if(i != 0)
        return i;
    
    result->value = merge8(lsb, result->value);
    bit_edit(&(result->flags), 7, 0 == result->value);
    bit_edit(&(result->flags), 6, 0);
    return ERR_NONE;
}

int alu_shift(alu_output_t* result, uint8_t x, rot_dir_t dir){
    if(result == NULL || dir < LEFT || dir > RIGHT)     // TODO: are enums unsigned?
        return ERR_BAD_PARAMETER;

    uint8_t left = 0;
    if(dir == LEFT){
        left = bit_get(x, 7);
        x <<= 1;
        result->value = x;
    }else{
        left = bit_get(x, 0);
        x >>= 1;
        result->value = x;
    }

    bit_edit(&(result->flags), 7, 0 == result->value);
    bit_edit(&(result->flags), 6, 0);
    bit_edit(&(result->flags), 5, 0);
    bit_edit(&(result->flags), 4, left);

    return ERR_NONE;
}

int alu_shiftR_A(alu_output_t* result, uint8_t x){
    if(result == NULL)
        return ERR_BAD_PARAMETER;

    bit_t msb = bit_get(x, 7);
    bit_t left = bit_get(x, 0);
    x >>= 1,
    x |= (msb << 7);
    result->value = x;

    bit_edit(&(result->flags), 7, 0 == result->value);
    bit_edit(&(result->flags), 6, 0);
    bit_edit(&(result->flags), 5, 0);
    bit_edit(&(result->flags), 4, left);

    return ERR_NONE;
}

int alu_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir){
    if(result == NULL || dir < LEFT || dir > RIGHT)
        return ERR_BAD_PARAMETER;

    uint8_t in = x;
    bit_t left = dir == LEFT ? bit_get(in, 7) : bit_get(in, 0);
    bit_rotate(&in, dir, 1);

    bit_edit(&(result->flags), 7, 0 == in);
    bit_edit(&(result->flags), 6, 0);
    bit_edit(&(result->flags), 5, 0);
    bit_edit(&(result->flags), 4, left);
    result->value = in;

    return ERR_NONE;

}

int alu_carry_rotate(alu_output_t* result, uint8_t x, rot_dir_t dir, flags_t flags){
    if(result == NULL || dir < LEFT || dir > RIGHT)
        return ERR_BAD_PARAMETER;

    bit_t s = dir == LEFT ? bit_get(x, 7) : bit_get(x, 0);
    
    if(dir == LEFT){
        x <<= 1;             //TODO: MAYBE SIMPILIFY WITH OTHAR METHODZ
        x |= bit_get(flags, 4);
    }else{
        x >>= 1;
        x |= get_C(flags);
    }

    result->value = x;
    bit_edit(&(result->flags), 7, 0 == result->value);
    bit_edit(&(result->flags), 6, 0);
    bit_edit(&(result->flags), 5, 0);
    bit_edit(&(result->flags), 4, s);

    return ERR_NONE;
}

bit_t get_flag(flags_t flags, flag_bit_t flag){
    switch(flag){
        case FLAG_Z: return bit_get(flags, 7) << 7;
        case FLAG_N: return bit_get(flags, 6) << 6;
        case FLAG_H: return bit_get(flags, 5) << 5;
        case FLAG_C: return bit_get(flags, 4) << 4;
        default: return ERR_NONE;
    }
}

void set_flag(flags_t* flags, flag_bit_t flag){
    switch(flag){
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