#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "adc.h"

void initAdc(gpio_irq_callback_t gpio_callback) {
    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(28);
    // Select ADC input 0 (GPIO26)
    adc_select_input(2);

    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE, true, gpio_callback);

}