
#include "mqtt.h"



void app_main(void)
{



    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta(); 
    mqtt_app_start(); 
    configure_led();  
    configure_oled();

}



//TODO save temperature log on device as well
//TODO make timed ISR to check temperature and update screen
//Make variables/macros for topics in mqttt file