
#include "ledconfig.h"
#define LED_STRIP_BLINK_GPIO  27
#define LED_STRIP_LED_NUMBERS 5

led_strip_handle_t led_strip = NULL;


led_strip_handle_t configure_led(void)
{
    // LED strip general initialization
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_BLINK_GPIO,   // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_NUMBERS,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags.invert_out = false,                // whether to invert the output signal
    };

    // LED strip backend configuration: SPI
    led_strip_spi_config_t spi_config = {
        .clk_src = SPI_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
        .flags.with_dma = true,         // Using DMA can improve performance and help drive more LEDs
        .spi_bus = SPI2_HOST,           // SPI bus ID
    };

    // LED Strip object handle
    led_strip_handle_t led_strip;
    led_strip_new_spi_device(&strip_config, &spi_config, &led_strip);
    return led_strip;
}



void toggle_led()
{
    static bool led_on_off = false;
        if (!led_on_off) {

            for (int i = 0; i < LED_STRIP_LED_NUMBERS; i++) {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 5, 5, 250)); //The fish like blue and i do too
            }

            ESP_ERROR_CHECK(led_strip_refresh(led_strip));

        } else {

            ESP_ERROR_CHECK(led_strip_clear(led_strip));

        }

        led_on_off = !led_on_off;
    
}