#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"

#define LED_STRIP_LED_NUMBERS 5

void toggle_led(void);
led_strip_handle_t configure_led(void);
void configureStrip(void);
void led_blue(void);
void led_red(void);
void led_off(void);