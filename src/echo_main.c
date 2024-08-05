
#include <stdio.h>
#include <string.h>
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

int main() {

    char str[25];

    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_set_hw_flow(uart0, false, false);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(uart0, false);

    uint br = spi_init(spi1, 992063);
    sprintf(str, "SPI speed %d\r\n", br);
    uart_puts(uart0, str);

    spi_set_slave(spi1, true);
    gpio_set_function(10, GPIO_FUNC_SPI);
    gpio_set_function(11, GPIO_FUNC_SPI);
    gpio_set_function(12, GPIO_FUNC_SPI);
    gpio_set_function(13, GPIO_FUNC_SPI);

    char in[16];

    while (true) {
            spi_read_blocking(spi1, 0xBA, in, 2);
            sprintf(str, "%d,%d ", in[0], in[1]);
            uart_puts(uart0, str);
    }

    return 0;
}