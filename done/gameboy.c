/**
 * @file gameboy.h
 * @brief Gameboy Header for GameBoy Emulator
 *
 * @author C. Hölzl, EPFL
 * @date 2019
 */

#include <stdint.h>
#include <stdlib.h>
#include "error.h"

#include "gameboy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a gameboy
 *
 * @param gameboy pointer to gameboy to create
 */
int gameboy_create(gameboy_t* gameboy, const char* filename);
if(gameboy == NULL || fil == NULL)
    return ERR_BAD_PARAMETER;

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 */
void gameboy_free(gameboy_t* gameboy);

/**
 * @brief Runs a gamefor for/until a given cycle
 */
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle);



#ifdef __cplusplus
}
#endif
