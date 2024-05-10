#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hwconf.h"
#include "comm.h"

void initComm(irq_handler_t on_uart_rx) {
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

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    gpio_init(CMD_DATA);
    gpio_set_dir(CMD_DATA, GPIO_OUT);

    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
}
