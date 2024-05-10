#include <stdlib.h>
#include "hwconf.h"
#include "display.h"
#include "graphics.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "pico/time.h"

static uint dma_tx;
static dma_channel_config c;
static uint printX = 0;
static uint printY = 0;
#define FB_SIZE (128 * 160 * 3)

void sendCommand(uint8_t cmd) {
    gpio_put(CMD_DATA, 0);
    spi_write_blocking(spi_default, &cmd, 1);
}

void sendData(uint8_t* data, size_t len) {
    gpio_put(CMD_DATA, 1);
    spi_write_blocking(spi_default, data, len);
}

void initDma() {
    dma_tx = dma_claim_unused_channel(true);

    c = dma_channel_get_default_config(dma_tx);
    channel_config_set_read_increment(&c, false);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, true));
}

void initDisplay() {
    // reset
    sendCommand(0x01);
    sleep_ms(120);

    // sleep out
    sendCommand(0x11);

    // display on
    sendCommand(0x29);

    initDma();
}

void clearDisplay() {
    static uint8_t white[] = {0xFF, 0xFF, 0xFF, 0xFF};

    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(spi_default)->dr, // write address
                          white,                        // read address
                          FB_SIZE,                      // element count (each element is of size transfer_data_size)
                          false);                       // don't start yet

    gpio_put(CMD_DATA, 1);
    sendCommand(0x2C);

    gpio_put(CMD_DATA, 1);
    dma_channel_transfer_from_buffer_now(dma_tx, white, FB_SIZE);
}

void setPrintCoords(uint x, uint y) {
    printX = x;
    printY = y;
}

void print(char* str, size_t len) {
    for (int i = 0; i < len && *str; i++) {
        uint8_t pixel_posX = printX * 8;
        uint8_t pixel_posY = printY * 8;

        uint8_t ch = *(str + i);
        uint8_t rangeX[] = {0, pixel_posX, 0, pixel_posX + 7};
        sendCommand(0x2A);
        sendData(rangeX, 4);

        uint8_t rangeY[] = {0, pixel_posY, 0, pixel_posY + 7};
        sendCommand(0x2B);
        sendData(rangeY, 4);

        uint8_t *arr = renderChar(ch);
        sendCommand(0x2C);
        sendData(arr, 64 * 3);
        free(arr);

        printX++;
        if (printX >= 16) {
            printX = 0;
            printY++;
        }

        if (printY >= 20) {
            printY = 0;
        }
    }

}