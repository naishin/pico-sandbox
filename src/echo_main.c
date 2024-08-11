
#include <stdio.h>
#include <string.h>
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

void spiRxISR() {
    char in[16];
    char* addr = in;
    while (spi_is_readable(spi1)) {
        spi_read_blocking(spi1, 0xBC, in, 1);
        addr++;
    }
}

int main() {

    char str[32];

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
    spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(10, GPIO_FUNC_SPI);
    gpio_set_function(11, GPIO_FUNC_SPI);
    gpio_set_function(12, GPIO_FUNC_SPI);
    gpio_set_function(13, GPIO_FUNC_SPI);

    // Enable the RX FIFO interrupt (RXIM)
    //spi1_hw->imsc = 1 << 2;
    spi_get_hw(spi1)->imsc |= SPI_SSPIMSC_RXIM_BITS;
    //hw_set_bits(&spi_get_hw(spi1)->imsc, SPI_SSPIMSC_RXIM_BITS);
    irq_set_enabled(SPI1_IRQ, true);
    irq_set_exclusive_handler(SPI1_IRQ, spiRxISR);

    int32_t cr0 = spi_get_hw(spi1)->cr0;
    sprintf(str, "spi1 cr0 %X\r\n", cr0);
    uart_puts(uart0, str);

    char in;

    while (true) {
        //spi_read_blocking(spi1, 0xBA, &in, 1);
        tight_loop_contents();
    }

    return 0;
}