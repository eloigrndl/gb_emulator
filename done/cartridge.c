/**
 * @file cartridge.c
 * @brief Game Boy Cartridge simulation header
 *
 * @author Eloi Garandel, Erik Wengle EPFL
 * @date 05/2020
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

/**
 * @brief Reads a file into the memory of a component
 *
 * @param c component to write to
 * @param filename file to read from
 * @return error code
 */
int cartridge_init_from_file(component_t* c, const char* filename){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(filename);

    FILE* file = fopen(filename, "rb");
    M_REQUIRE_NON_NULL_CUSTOM_ERR(file, ERR_IO);
    M_EXIT_IF_ERR_DO_SOMETHING(c->mem->memory == NULL ? ERR_IO : ERR_NONE, fclose(file));
    M_EXIT_IF_ERR_DO_SOMETHING((ferror(file) || fread(c->mem->memory, 1, BANK_ROM_SIZE, file) < BANK_ROM_SIZE) ? ERR_IO : ERR_NONE, fclose(file));
    M_EXIT_IF_ERR_DO_SOMETHING((c->mem->memory[CARTRIDGE_TYPE_ADDR] != 0) ? ERR_NOT_IMPLEMENTED : ERR_NONE, fclose(file));

    fclose(file);


    return ERR_NONE;
}


/**
 * @brief Initiates a cartridge given a filename
 *
 * @param ct cartridge to initiate
 * @param filename file to read from
 * @return error code
 */
int cartridge_init(cartridge_t* ct, const char* filename){
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(filename);

    M_REQUIRE_NO_ERR(component_create(&(ct->c), BANK_ROM_SIZE));
    M_REQUIRE_NO_ERR(cartridge_init_from_file(&(ct->c), filename));
    return ERR_NONE;
}


/**
 * @brief Plugs a cartridge to the bus
 *
 * @param ct cartridge to plug
 * @param bus bus to plug into
 * @return error code
 */
int cartridge_plug(cartridge_t* ct, bus_t bus){
    M_REQUIRE_NON_NULL(ct);
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NO_ERR(bus_forced_plug(bus, &(ct->c), BANK_ROM0_START, BANK_ROM1_END, 0));
    return ERR_NONE;
}


/**
 * @brief Frees a cartridge
 *
 * @param ct cartridge to free
 */
void cartridge_free(cartridge_t* ct){
    if(ct == NULL) return;

   component_free(&(ct->c));
   return;
}

#ifdef __cplusplus
}
#endif
