#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "pico/types.h"
#include <stdint.h>

extern const size_t FB_SIZE;

void fill(uint8_t* data, uint8_t red, uint8_t green, uint8_t blue);
uint8_t* renderChar(uint8_t);

#endif