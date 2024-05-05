#include "pico/platform.h"
#include "graphics.h"


const size_t FB_SIZE = 128 * 160 * 3;

uint8_t __in_flash("font") font [] = {
 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00
};

void fill(uint8_t* data, uint8_t red, uint8_t green, uint8_t blue) {
    for(int y = 0; y < 160; y++) {
        for(int x = 0; x < 128; x++) {
            int index = (y * 128 + x) * 3 ;
            data[index] = red;
            data[index + 1] = green;
            data[index + 2] = blue;
        }
    }
}

void drawA(uint8_t* data) {
    for(int y = 0; y < 20; y++) {
        int offsetY = y * 8 * 16 * 8;
        for(int x = 0; x < 16; x++) {
            int offsetX = x * 8;
            for(int py = 0; py < 8; py++) {
                uint8_t line = font[py];
                for(int px = 0; px < 8; px++) {
                    int offset = (offsetY + offsetX + py * 128 + px) * 3;
                    if( (line & 1) == 1) {
                        data[offset] = 0x00;
                        data[offset + 1] = 0x00;
                        data[offset + 2] = 0x00;
                    }
                    line = line >> 1;
                }
            }
        }
    }
}
