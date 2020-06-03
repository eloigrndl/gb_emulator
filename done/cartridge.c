/**
 * @file cartridge.c
 * @brief Game Boy Cartridge simulation header
 *
 * @author Eloi Garandel, Erik Wengle EPFL
 * @date 2020
 */

#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "bus.h"
#include "cartridge.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==== see cartridge.h ========================================
int cartridge_init_from_file(component_t* c, const char* filename){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(filename);

    FILE* file = fopen(filename, "rb");
    M_REQUIRE_NON_NULL_CUSTOM_ERR(file, ERR_IO);

    fseek(file, 0, SEEK_END); // seek to end of file
    size_t size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    
    M_EXIT_IF_ERR_DO_SOMETHING(c->mem->memory == NULL ? ERR_IO : ERR_NONE, fclose(file));
    M_EXIT_IF_ERR_DO_SOMETHING((ferror(file) || fread(c->mem->memory, 1, size, file) < size) ? ERR_IO : ERR_NONE, fclose(file));
    M_EXIT_IF_ERR_DO_SOMETHING((c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0) ? ERR_NOT_IMPLEMENTED : ERR_NONE, fclose(file));

    fclose(file);


    return ERR_NONE;
}


// ==== see cartridge.h ========================================
int cartridge_init(cartridge_t* ct, const char* filename){
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(filename);

    M_REQUIRE_NO_ERR(component_create(&(ct->c), BANK_ROM_SIZE));

    error_code e = ERR_NONE;
    if((e = cartridge_init_from_file(&(ct->c), filename)) != ERR_NONE){
        component_free(&(ct->c));
    }
    return e;
}


// ==== see cartridge.h ========================================
int cartridge_plug(cartridge_t* ct, bus_t bus){
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NO_ERR(bus_forced_plug(bus, &(ct->c), BANK_ROM0_START, BANK_ROM1_END, 0));
    return ERR_NONE;
}



// ==== see cartridge.h ========================================
void cartridge_free(cartridge_t* ct){
    if(ct == NULL) return;

   component_free(&(ct->c));
   return;
}

#ifdef __cplusplus
}
#endif
