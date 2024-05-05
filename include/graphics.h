#ifndef PROJECT1_GRAPHICS
#define PROJECT1_GRAPHICS 

#include "pico/types.h"
#include <stdint.h>

extern const size_t FB_SIZE;

extern uint8_t font[];

void fill(uint8_t* data, uint8_t red, uint8_t green, uint8_t blue);
void drawA(uint8_t* data);

#endif