#include <stdint.h>
#include "sidlib.h"
#include "lcdc.h"
#include "gameboy.h"
#include "error.h"
#include "util.h"
#include <sys/time.h> 
// Key press bits
#define MY_KEY_UP_BIT       0x01
#define MY_KEY_DOWN_BIT     0x02
#define MY_KEY_RIGHT_BIT    0x04
#define MY_KEY_LEFT_BIT     0x08
#define MY_KEY_A_BIT        0x10
#define MY_KEY_B_BIT        0X20
#define MY_KEY_START_BIT    0x30
#define MY_KEY_SELECT_BIT   0x40

#define SCALE_FACTOR     4
gameboy_t gb;
struct timeval start;
struct timeval paused;

/**
 * @brief Sets a pixel from a two-dimensional set of pixels with an indicated grey value
 *
 * @param pixels Set of pixels
 * @param row row index
 * @param col column index
 * @param width with of a single row
 * @param grey indiccated grey value
 */
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    if(pixels == NULL) 
        return;

    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

/**
 * @brief Computes how many gameboy cycles have elapsed since the given period
 *
 * @param from Original period
 * @return Number of gameboy cycles that have elapsed
 */
uint64_t get_time_in_GB_cycles_since(struct timeval* from)
{
    if(from == NULL)
        return 0;

    struct timeval now;
    gettimeofday(&now, NULL);

    if(!timercmp(&now, from, >)){
        return 0;
    }
    
    struct timeval delta;
    timersub(&now, from, &delta);
    uint64_t v =  delta.tv_sec * GB_CYCLES_PER_S + (delta.tv_usec * GB_CYCLES_PER_S) / 1000000;
    return v;
}

/**
 * @brief Generates an image for the gameboys screen
 *
 * @param pixels set of pixels to be set
 * @param height height of the window to be displayed
 * @param width width of the window to be displayed
 */
static void generate_image(guchar* pixels, int height, int width)
{   
    if(pixels == NULL)
        return;
    
    if(gameboy_run_until(&gb, get_time_in_GB_cycles_since(&start)) != ERR_NONE)
        return;

    uint8_t pixelval = 0;
    for(int i = 0; i < width; ++i){
        for(int j = 0; j < height; ++j){
            image_get_pixel(&pixelval, &(gb.screen.display), i/SCALE_FACTOR, j/SCALE_FACTOR);
            set_grey(pixels,  j, i, width, 255 - 85 * pixelval);
        }
    } 
}

// registers a key-press
#define do_key(X) \
    do { \
        if (! (psd->key_status & MY_KEY_ ## X ##_BIT)) { \
            psd->key_status |= MY_KEY_ ## X ##_BIT; \
            puts(#X " key pressed"); \
        } \
    } while(0)

/**
 * @brief Handles pressed keyboard keys
 *
 * @param keyval key that has been pressed
 * @param data pointer of datahandler
 * @return whether a keypress has been detected or not
 */
static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), UP_KEY));
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), DOWN_KEY));
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), RIGHT_KEY));
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), LEFT_KEY));
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), A_KEY));
        return TRUE;

    case 'S':
    case 's':
        do_key(B);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), B_KEY));
        return TRUE;

    case 'K':
    case 'k':
        do_key(START);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), START_KEY));
        return TRUE;
    
    case 'L':
    case 'l':            
        do_key(SELECT);
        M_REQUIRE_NO_ERR(joypad_key_pressed(&(gb.pad), SELECT_KEY));
        return TRUE;
    case GDK_KEY_space:
        {
            puts("PAUSE key pressed");
            struct timeval now;
            gettimeofday(&now, NULL);
            timersub(&now, &paused, &paused);
            timeradd(&start, &paused, &start);
            timerclear(&paused);
        }
    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key



// registers a key-release
#define do_key(X) \
    do { \
        if (psd->key_status & MY_KEY_ ## X ##_BIT) { \
          psd->key_status &= (unsigned char) ~MY_KEY_ ## X ##_BIT; \
            puts(#X " key released"); \
        } \
    } while(0)


/**
 * @brief Handles released keyboard keys
 *
 * @param keyval key that has been pressed
 * @param data pointer of datahandler
 * @return whether a keypress has been detected or not
 */
static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), UP_KEY));
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), DOWN_KEY));
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), RIGHT_KEY));
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), LEFT_KEY));
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), A_KEY));
        return TRUE;

    case 'S':
    case 's':
        do_key(B);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), B_KEY));
        return TRUE;

    case 'K':
    case 'k':
        do_key(START);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), START_KEY));
        return TRUE;
    
    case 'L':
    case 'l':            
        do_key(SELECT);
        M_REQUIRE_NO_ERR(joypad_key_released(&(gb.pad), SELECT_KEY));
        return TRUE;
    }

    return FALSE;
}
#undef do_key

/**
 * @brief Prints a message in case of an error at launch
 *
 * @param pgm launched program
 * @param msg error message to be displayed
 */
static void error(const char* pgm, const char* msg)
{
    fputs("ERROR: ", stderr);
    if (msg != NULL) fputs(msg, stderr);
    fprintf(stderr, "\nusage:    %s input_file\n", pgm);
    fprintf(stderr, "example: %s rom.gb\n", pgm);
}


/**
 * @brief Main method of the gameboy emulator
 *
 * @param argc argument count
 * @param argv argument values
 * @return error code
 */
int main(int argc, char *argv[])
{

    if (argc < 2) {
        error(argv[0], "please provide input_file");
        return 1;
    }

    const char* const filename = argv[1];
    zero_init_var(gb);

    int err = gameboy_create(&gb, filename);
    if (err != ERR_NONE) {
        gameboy_free(&gb);
        return err;
    }

    timerclear(&paused);
    gettimeofday(&start, NULL);


    sd_launch(&argc, &argv, sd_init("Gameboy", LCD_WIDTH * SCALE_FACTOR, LCD_HEIGHT * SCALE_FACTOR, 40,
                        generate_image, keypress_handler, keyrelease_handler));

    gameboy_free(&gb);

    return err;
    
}
