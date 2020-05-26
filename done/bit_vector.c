/**
 * @file bit_vector.c
 * @brief Bit vectors for PPS projects
 * @author Eloi Garandel, Erik Wengle EPFL
 * @date 2020
 */

#include "bit.h"
#include "bit_vector.h"
#include "error.h"
#include <stddef.h> // for size_t
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "myMacros.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_create(size_t size, bit_t value){
    if(size == 0 || size > UINT32_MAX)  //FIXME what is max value??
        return NULL;

    size_t rounded_size = size;
    if(rounded_size % 32 != 0)
        rounded_size += 32 - (rounded_size % 32);

    bit_vector_t* res = malloc(sizeof(bit_vector_t));
    
    if(res == NULL) return NULL;

    res->content = calloc(rounded_size / 32 , sizeof(uint32_t));
    if(res->content == NULL) return NULL;

    res->size = size;
    res->nb_fields = rounded_size / 32;

    size_t fill = (value != 0) ? 0xFFFFFFFF : 0;
    
    for(int i = 0; i < res->nb_fields; ++i){
        res->content[i] = fill;
    }
    res->content[res->nb_fields - 1] = (BIT_MASK32(size) & fill);

    return res;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv){
    if(pbv == NULL) 
        return NULL;
    
    bit_vector_t* res = bit_vector_create(pbv->size, 0);
    
    for(int i = 0; i < pbv->nb_fields; ++i){
        res->content[i] = pbv->content[i];
    }
    return res;
}

// ==== see bit_vector.h ========================================
bit_t bit_vector_get(const bit_vector_t* pbv, size_t index){
    return (pbv == NULL || index >= pbv->size || index < 0) ? 0 : bit_get(pbv->content[index/32] >> ((index % 32 / 8) * 8), index % 8);
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_not(bit_vector_t* pbv){
    if(pbv == NULL) return NULL;

    for(int i = 0; i < pbv->nb_fields; ++i){
        pbv->content[i] = ~(pbv->content[i]);
    }

    REMOVE_TAIL32(pbv);

    return pbv;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size) 
        return NULL;

    for(int i = 0; i < pbv1->nb_fields; ++i)
        pbv1->content[i] &= pbv2->content[i];

    REMOVE_TAIL32(pbv1);
    
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
        return NULL;


    for(int i = 0; i < pbv1->nb_fields; ++i)
        pbv1->content[i] |= pbv2->content[i];

    REMOVE_TAIL32(pbv1);
    
    return pbv1; 
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2){
   if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
        return NULL;

    for(int i = 0; i < pbv1->nb_fields; ++i)
        pbv1->content[i] ^= pbv2->content[i];

    REMOVE_TAIL32(pbv1);
    
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if(size == 0 || pbv == NULL) //FIXME: WHAT DO YOU WANT FROM US??????? 
        return NULL;

    bit_vector_t* res = bit_vector_create(size, 0); 
    int to_extract = size;

    if(index < 0){

        for(int i = -index; i < size; ++i){
            if(i+index >= pbv->size){
                continue;
            } else {
                if((pbv->content[(i+index)/32] & (1 << (i+index))) != 0)
                    res->content[i/32] |= (1 << (i%32));
            }

            
        }
    } else {
        for(int i = 0; i < size; ++i){    
            if(i+index >= pbv->size){
                continue;       
            } else {   
                if((pbv->content[(i+index)/32] & (1 << (i+index))) != 0)
                    res->content[i/32] |= (1 << (i%32)); 
            }               
        }         
    }
    REMOVE_TAIL32(res);
    return res;    
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if(size == 0 || pbv == NULL) //FIXME: WHAT DO YOU WANT FROM US??????? 
        return NULL;

    bit_vector_t* res = bit_vector_create(size, 0); 

    if(index < 0){

        int64_t newIndex = index%pbv->size + pbv->size;
    }

    for(int i = 0; i < size; ++i){
        if((pbv->content[((i+index)%pbv->size)/32] & 1 << ((i+index)%pbv->size)) != 0){
            res->content[i/32] |= (1 << (i%32));
        }
    }
    

    return res;
}


/*
m5:
{0xD5B60ABB, 0xD5B60ABB}
orig: 1101 1110 1010 1101 1011 0000 0101 0101, 1101 1110 1010 1101 1011 0000 0101 0101
need: 1101 0101 1011 0110 0000 1010 1011 1011, 1101 0101 1011 0110 0000 1010 1011 1011
have: 1101 0101 1011 0110 0000 1010 1011 1011, 1101 0101 1011 0110 0000 1010 1011 0101
m10:
orig: 1101 1110 1010 1101 1011 0000 0101 0101, 1101 1110 1010 1101 1011 0000 0101 0101
need: 1011 0110 1100 0001 0101 0111 0111 1010, 1011 0110 1100 0001 0101 0111 0111 1010
have: 
*/

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift){
    if(pbv == NULL) return NULL;
    
    return bit_vector_extract_zero_ext(pbv, -shift, pbv->size);
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift){
    if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size || shift < 0 || shift > pbv1->size ) {
        return NULL;
    }

    bit_vector_t* res = bit_vector_create(pbv1->size, 0);

  ;  for(int i = 0; i < pbv1->size; i++){
        uint32_t index = i%32;

        //pbv1 values
        if(i < shift){
            if((pbv1->content[i/32] & (1 << index)) != 0){
                res->content[i/32] |= (1 << index);
            }

        } else{
            if((pbv2->content[i/32] & (1 << index)) != 0){
                res->content[i/32] |= (1 << index);
            }
        }
    }
    bit_vector_println("res  : ", res);

    return res;
}

// ==== see bit_vector.h ========================================
int bit_vector_print(const bit_vector_t* pbv){
    if(pbv == NULL) 
        return 0;

    for(int i = pbv->size - 1; i >= 0; i--){
        printf("%d", bit_vector_get(pbv, i));
    }

    return pbv->size;
}
// ==== see bit_vector.h ========================================
int bit_vector_println(const char* prefix, const bit_vector_t* pbv){
    if(pbv == NULL || prefix == NULL) 
        return 0;
    
    printf("%s ", prefix);
    size_t size = bit_vector_print(pbv);
    printf("\n");
    return size;
}

// ==== see bit_vector.h ========================================
void bit_vector_free(bit_vector_t** pbv){
    free((*pbv)->content);
    free(*pbv);
    return;
}

