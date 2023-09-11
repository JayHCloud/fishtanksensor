
#include "connectwifi.h"
#include "mqtt.h"
#include "watertemp.h"
#include "led_strip.h"
#include "esp_log.h"
#include "ledconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include ""



void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta(); // Initialize Wifi Event Handler
    mqtt_app_start(); // Initialize Mqtt Event Handler
    configureStrip();  //Configure LED strip settings
}



