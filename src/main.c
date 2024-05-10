#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "boards/pico.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "pico/types.h"
#include "pico/platform.h"
#include "graphics.h"

#define LED PICO_DEFAULT_LED_PIN
#define CMD_DATA 20
#define BUTTON 2

#define UART_TX_PIN 0
#define UART_RX_PIN 1

void sendCommand(uint8_t);
void sendData(uint8_t*, size_t);

uint dma_tx;
dma_channel_config c;
static char event_str[128];

// RX interrupt handler
void on_uart_rx() {
    gpio_put(LED, 1);
    while (uart_is_readable(uart0)) {
        static int pos = 0;

        uint8_t ch = uart_getc(uart0);
        uart_putc(uart0, ch);

        int posX = pos % 16;
        int posY = pos / 16;
        uint8_t pixel_posX = posX * 8;
        uint8_t pixel_posY = posY * 8;

        uint8_t coord[] = {0, pixel_posX, 0, pixel_posX + 7};
        sendCommand(0x2A);
        sendData(coord, 4);

        uint8_t coord2[] = { 0, pixel_posY, 0, pixel_posY + 7};
        sendCommand(0x2B);
        sendData(coord2, 4);

        uint8_t* arr = renderChar(ch);
        sendCommand(0x2C);
        sendData(arr, 64 * 3);
        free(arr);

        pos++;
        if (pos == (16 * 20)) {
            pos = 0;
        }
    }
    gpio_put(LED, 0);
}

void gpio_event_string(char *buf, uint32_t events);

static bool readAdc = false;

void gpio_callback(uint gpio, uint32_t events) {
    readAdc = !readAdc;
}

void initComm() {
    // Enable UART
    // Set up our UART with a basic baud rate.
    uart_init(uart0, 115200);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uart0, false, false);

    // Set our data format
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(uart0, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uart0, true, false);

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

    gpio_init(CMD_DATA);
    gpio_set_dir(CMD_DATA, GPIO_OUT);

    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
}

void initDma() {
    dma_tx = dma_claim_unused_channel(true);

    c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_dreq(spi_default, true));
}

void sendCommand(uint8_t cmd) {
    gpio_put(CMD_DATA, 0);
    spi_write_blocking(spi_default, &cmd, 1);
}

void sendData(uint8_t* data, size_t len) {
    gpio_put(CMD_DATA, 1);
    spi_write_blocking(spi_default, data, len);
}

int main() {


    uint8_t *fbdata = malloc(FB_SIZE);

    initComm();
    initDma();

    // reset
    sendCommand(0x01);
    sleep_ms(120);

    // sleep out
    sendCommand(0x11);

    // display on
    sendCommand(0x29);

    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(spi_default)->dr, // write address
                          fbdata,                       // read address
                          FB_SIZE,                      // element count (each element is of size transfer_data_size)
                          false);                       // don't start yet

    fill(fbdata, 0xFF, 0x80, 0xFF);

    sendCommand(0x2C);

    gpio_put(CMD_DATA, 1);
    dma_channel_transfer_from_buffer_now(dma_tx, fbdata, FB_SIZE);

    uart_puts(uart0, "\r\n I'm ready\r\n");

    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(28);
    // Select ADC input 0 (GPIO26)
    adc_select_input(2);

    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    uint8_t* str = malloc(100);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);

    while (1) {
        if(readAdc) {
            uint16_t result = adc_read();
            sprintf(str, "Raw value: 0x%03x, voltage: %f V\n\r", result, result * conversion_factor);
            uart_puts(uart0, str);
        }
        sleep_ms(500);
    }

    while(true) {
        tight_loop_contents();
    }
}
