#ifndef DISPLAY_H
#define DISPLAY_H

#include <stddef.h>
#include <stdint.h>
#include "pico/types.h"

void initDisplay();
void clearDisplay();
void sendCommand(uint8_t);
void sendData(uint8_t*, size_t);
void setPrintCoords(uint, uint);
void print(char*, size_t);

#endif
