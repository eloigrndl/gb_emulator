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

#ifdef __cplusplus
extern "C" {
#endif

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_create(size_t size, bit_t value){

    if(size % 32 != 0)
        size += 32 - (size % 0);
    
    bit_vector_t* res = malloc(sizeof(bit_vector_t) + size * sizeof(uint32_t));
    if(res == NULL)
        return NULL;

    if(value == 1)
        value = 0xFFFFFFFF;
    else 
        value = 0;

    for(int i = 0; i < size/32; ++i){
        res->content[i] = value;
    }
    return res;

}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_cpy(const bit_vector_t* pbv){
    
    bit_vector_t* res = malloc(sizeof(bit_vector_t) + pbv->size * sizeof(uint32_t));

    if(res == NULL)
        return NULL;
    
    for(int i = 0; i < pbv->size/32; ++i){
        res->content[i] = pbv->content[i];
    }
    return res;
}

// ==== see bit_vector.h ========================================
bit_t bit_vector_get(const bit_vector_t* pbv, size_t index){
    M_REQUIRE_NON_NULL(pbv);
    if(index >= pbv->size)
        return 0;

    return (pbv->content[index/32] && 1 << index % 32) >> index % 32;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_not(bit_vector_t* pbv){
    M_REQUIRE_NON_NULL(pbv);

    for(int i = 0; i < pbv->size/32 -1; ++i)
        pbv->content[i] = ~(pbv->content[i]);
    //FIXME: CASE OF END OF VECTOR 
    
    return pbv;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_and(bit_vector_t* pbv1, const bit_vector_t* pbv2){
    if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
        return NULL;
    
    for(int i = 0; i < pbv1->size/32; ++i)
        pbv1->content[i] &= pbv2->content[i];
    
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_or(bit_vector_t* pbv1, const bit_vector_t* pbv2){
     if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
        return NULL;
    
    for(int i = 0; i < pbv1->size/32; ++i)
        pbv1->content[i] |= pbv2->content[i];
    
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_xor(bit_vector_t* pbv1, const bit_vector_t* pbv2){
     if(pbv1 == NULL || pbv2 == NULL || pbv1->size != pbv2->size)
        return NULL;
    
    for(int i = 0; i < pbv1->size/32; ++i)
        pbv1->content[i] ^= pbv2->content[i];
    
    return pbv1;
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_zero_ext(const bit_vector_t* pbv, int64_t index, size_t size){
    if(size == 0)
        return NULL;
        
//FIXME DIDNT UNDERSTAND
    
}

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_extract_wrap_ext(const bit_vector_t* pbv, int64_t index, size_t size);
//FIXME DIDNT UNDERSTAND

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_shift(const bit_vector_t* pbv, int64_t shift);

// ==== see bit_vector.h ========================================
bit_vector_t* bit_vector_join(const bit_vector_t* pbv1, const bit_vector_t* pbv2, int64_t shift){
    M_REQUIRE_NON_NULL(pbv1);
    M_REQUIRE_NON_NULL(pbv2);

    bit_vector_t* res = malloc(sizeof(bit_vector_t) + (pbv1->size + pbv2->size) * sizeof(uint32_t));
    if(res == NUL) 
        return NULL;

    size_t nb = shift / 32;
    for(int i = 0; i < nb; ++i){
        res->content[i] = pbv1->content[i];
    }

    uint32_t index = shift - 32 * nb;
    uint32_t lowMask = (1 << index) - 1;
    uint32_t highMask = ~lowMask;
    res->content[nb] = (pbv1->content[nb] && lowMask) | ((pbv2->content[nb] && highMask) << index));

    for(int i = nb + 1; i < (pbv1->size + pbv2->size)/32; ++i){
        res[nb] = pbv2->content[nb];
    }
}

// ==== see bit_vector.h ========================================
int bit_vector_print(const bit_vector_t* pbv){
    M_REQUIRE_NON_NULL(pbv);

    size_t size = 0;
    for(int i = size - 1; i >= 0; --i){
        printf("%b", pbv->content[i]);
        ++size;
    }
    return size;
}

// ==== see bit_vector.h ========================================
int bit_vector_println(const char* prefix, const bit_vector_t* pbv){
    M_REQUIRE_NON_NULL(pbv);
    M_REQUIRE_NON_NULL(prefix);
    
    printf("%s ", prefix);
    size_t size = bit_vector_print(pbv);
    printf("\n");
    return size;
}

// ==== see bit_vector.h ========================================
void bit_vector_free(bit_vector_t** pbv);

