
#pragma once 
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "include/mqtt.h"

typedef uint8_t DeviceAddress[8];
typedef uint8_t ScratchPad[9];


esp_err_t ds18b20_init(int GPIO);
esp_err_t ds18b20_reset(void);


void ds18b20_write(char bit);
void ds18b20_write_byte(char data);

unsigned char ds18b20_read(void);
unsigned char ds18b20_read_byte(void);


float ds18b20_get_temp(void);
const char* ds18b20_get_temp_s(void);
void ds18b20_send_mqtt(void);
