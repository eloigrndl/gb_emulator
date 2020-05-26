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

    bit_vector_print(pbv);

    bit_vector_t* res = bit_vector_create(size, 0); 
    int to_extract = size;

    if(index < 0){
        int fieldId = index < 0 ? 0 : index/32;
        int j = index;

        while(j <= -32){
            res->content[fieldId] = 0;
            fieldId++;
            to_extract -= 32;
            j += 32;
        }

        int k = index % 32;
        if (k < 0)
            k += 32;


        int i = 0;
        uint32_t rest = 0;

        while (to_extract > 0){
            res->content[fieldId+i] = (pbv->content[i] << (32-k)) | rest;
            rest = (pbv->content[i] & BIT_MASK32_INV(32-k)) >> (k);
            i++;
            to_extract -= 32;
        }

    } else {
        
        int k = index % 32;
        int fieldId = index / 32;
        uint32_t rest = fieldId < pbv->nb_fields - 1 ? (((pbv->content[fieldId +1] & BIT_MASK32(k)) << (32-k) ) & BIT_MASK32(32)): 0;

        int i = fieldId;
        while(i < res->nb_fields){
            res->content[i-fieldId] = pbv->content[i] >> k | rest;
            ++i;
            rest = i < (pbv->nb_fields - 1) ? ((pbv->content[fieldId+1] & BIT_MASK32(k)) << (32-k)) : 0;
        }

        while(i < size / 32){
            res->content[i] = 0;
            i++;
        }
    }
    REMOVE_TAIL32(res);
    return res;    
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if(size == 0 || pbv == NULL) //FIXME: WHAT DO YOU WANT FROM US??????? 
        return NULL;

    printf("starting to wrap ext\n");

    bit_vector_t* res = bit_vector_create(size, 0); 
    int to_extract = size;
    int originalField = (index % pbv->size) / 32;



    if(index < 0){
        int fieldId = 0;
        int j = index;

        printf("new vector created : nb fields : %d + index : %d\n", res->nb_fields, index);

        while(fieldId < pbv->nb_fields && j <= -32 ){
            printf("j : %d / fieldId : %d\n", j, fieldId);
            res->content[fieldId] = pbv->content[(originalField + fieldId)];
            printf("what is the thing : %u \n", pbv->content[(originalField + fieldId)]);
            fieldId++;
            to_extract -= 32;
            j += 32;
        }
        printf("left to extract : %d \n", to_extract);

        int k = index % 32;
        if (k < 0)
            k += 32;

        printf("starting to fill the content of the new vec \n");
        bit_vector_print(res);
        putchar('\n');
        bit_vector_print(pbv);
        putchar('\n');

        int i = 0;
        uint32_t rest = (pbv->content[i] & BIT_MASK32_INV(32-k)) >> (k);

        while (to_extract > 0){
            res->content[fieldId+i] = (pbv->content[(originalField + fieldId + i - 1) % pbv->nb_fields] << (32-k)) | rest;
            rest = (pbv->content[i%(pbv->nb_fields)] & BIT_MASK32_INV(32-k)) >> (k);

            i++;
            to_extract -= 32;
        }

    } else {

        
        int k = index % 32;
        int fieldId =  (index % pbv->size) / 32;
        uint32_t rest = (((pbv->content[(fieldId + 1) % pbv->nb_fields] & BIT_MASK32(k)) << (32-k) )) & BIT_MASK32(32);

        int i = fieldId;



        while(i < res->nb_fields){
            printf("%d < %d\n", i, res->nb_fields);
            res->content[i-fieldId] = pbv->content[i%(pbv->nb_fields)] >> k | rest;
            ++i;
            rest = ((pbv->content[(fieldId+1) % pbv->nb_fields] & BIT_MASK32(k)) << (32-k));
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

        bit_vector_t* res = bit_vector_cpy(pbv1);
        
        if(shift == pbv1->size) 
            return res;
        
        res->content[shift / 32] &= BIT_MASK32((shift % 32));
        res->content[shift / 32] |= ((pbv2->content[shift / 32] & BIT_MASK32(32 - shift)) << shift);

    for(int i = shift / 32 + 1; i < pbv1->nb_fields; i++){
        res->content[i] = pbv2->content[i];
    }

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
    printf("YOOOOOOOOOOO INT FREEEEEEEE \n");
    free((*pbv)->content);
    free(*pbv);
    return;
}

