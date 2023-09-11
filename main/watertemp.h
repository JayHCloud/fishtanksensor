
#pragma once 
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


typedef uint8_t DeviceAddress[8];
typedef uint8_t ScratchPad[9];


esp_err_t ds18b20_init(int GPIO);
esp_err_t ds18b20_reset(void);


float ds18b20_get_temp(void);



