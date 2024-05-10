#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boards/pico.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/adc.h"
#include "pico/types.h"
#include "pico/platform.h"
#include "pico/float.h"
#include "hwconf.h"
#include "graphics.h"
#include "comm.h"
#include "adc.h"
#include "display.h"

static char event_str[128];
static bool readAdc = false;

// RX interrupt handler
void on_uart_rx() {
    gpio_put(LED, 1);
    while (uart_is_readable(uart0)) {
        char ch = uart_getc(uart0);
        uart_putc(uart0, ch);

        setPrintCoords(8, 10);
        print(&ch, 1);
     }
    gpio_put(LED, 0);
}

void gpio_callback(uint gpio, uint32_t events) {
    readAdc = !readAdc;
}

int main() {

    //setup uart
    //setup spi
    //setup gpio
    initComm(on_uart_rx);

    //setup screen
    //clear screen
    //set up adc
    //set up timer
    //output measured value

    initDisplay();
    clearDisplay();

    initAdc(gpio_callback);

    uart_puts(uart0, "\r\n I'm ready\r\n");

    uint8_t* str = malloc(100);

    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    const float conversion_factor = 3.3f / (1 << 12);

    while (1) {
        if(readAdc) {
            uint16_t result = adc_read();
            sprintf(str, "V0: %4.2f", result * conversion_factor);
            setPrintCoords(2, 10);
            print(str, strlen(str));

            sprintf(str, "Raw value: 0x%03x, voltage: %f V\n\r", result, result * conversion_factor);
            uart_puts(uart0, str);
        }
        sleep_ms(500);
    }

    while(true) {
        tight_loop_contents();
    }
}
