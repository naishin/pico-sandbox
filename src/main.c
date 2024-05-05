#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "boards/pico.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "pico/types.h"
#include "pico/platform.h"
#include "graphics.h"

#define LED PICO_DEFAULT_LED_PIN
#define CMD_DATA 20

void initComm() {
    // Enable UART so we can print
    stdio_init_all();

    // Enable SPI 0 at 1 MHz and connect to GPIOs
    spi_init(spi_default, 62 * 1000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_set_function(CMD_DATA, GPIO_FUNC_SIO);
    gpio_set_dir(CMD_DATA, GPIO_OUT);
}

int main() {

    uint8_t *fbdata = malloc(FB_SIZE);

    initComm();

    const uint dma_tx = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    uint dreq = spi_get_dreq(spi_default, true);
    channel_config_set_dreq(&c, dreq);

    // Write the output buffer to MOSI, and at the same time read from MISO.
    //spi_write_read_blocking(spi_default, out_buf, in_buf, BUF_LEN);
    uint8_t command[1];
    gpio_put(CMD_DATA, 0);

    // reset
    command[0] = 0x01;
    spi_write_blocking(spi_default, command, 1);
    sleep_ms(120);

    // sleep out
    command[0] = 0x11;
    spi_write_blocking(spi_default, command, 1);

    // display on
    command[0] = 0x29;
    spi_write_blocking(spi_default, command, 1);

    command[0] = 0x2c;

    while(true) {
        gpio_put(LED, 1);
        fill(fbdata, 0xFC, 0x00, 0x00);
        drawA(fbdata);
        gpio_put(LED, 0);
        gpio_put(CMD_DATA, 0);
        spi_write_blocking(spi_default, command, 1);
        gpio_put(CMD_DATA, 1);
        //spi_write_blocking(spi_default, fbdata, FB_SIZE);
        dma_channel_configure(dma_tx, &c,
                            &spi_get_hw(spi_default)->dr, // write address
                            fbdata, // read address
                            FB_SIZE, // element count (each element is of size transfer_data_size)
                            false); // don't start yet
        dma_channel_start(dma_tx);
        sleep_ms(1*1000);

        gpio_put(LED, 1);
        fill(fbdata, 0x00, 0xFC, 0x00);
        drawA(fbdata);
        gpio_put(LED, 0);
        gpio_put(CMD_DATA, 0);
        spi_write_blocking(spi_default, command, 1);
        gpio_put(CMD_DATA, 1);
        //spi_write_blocking(spi_default, fbdata, FB_SIZE);
        dma_channel_configure(dma_tx, &c,
                              &spi_get_hw(spi_default)->dr, // write address
                              fbdata,                       // read address
                              FB_SIZE, // element count (each element is of size transfer_data_size)
                              false);  // don't start yet
        dma_channel_start(dma_tx);
        sleep_ms(1*1000);

        gpio_put(LED, 1);
        fill(fbdata, 0x00, 0x00, 0xFC);
        drawA(fbdata);
        gpio_put(LED, 0);
        gpio_put(CMD_DATA, 0);
        spi_write_blocking(spi_default, command, 1);
        gpio_put(CMD_DATA, 1);
        //spi_write_blocking(spi_default, fbdata, FB_SIZE);
        dma_channel_configure(dma_tx, &c,
                        &spi_get_hw(spi_default)->dr, // write address
                        fbdata,                       // read address
                        FB_SIZE, // element count (each element is of size transfer_data_size)
                        false);  // don't start yet
        dma_channel_start(dma_tx);
        sleep_ms(1*1000);
    }
}

