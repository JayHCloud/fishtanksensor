
#include "include/mqtt.h"
#include "include/tasks.h"
#include "include/connectwifi.h"
#include "include/ledconfig.h"


void app_main(void)
{

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta(); 
    mqtt_app_start(); 
    configure_led();  
    tasks_init(); 

}


//TODO Make Generic LED func that takes mqtt? input and changes to colour based on RBG values given
//TODO Look into PH sensor
//TODO Task for light effects?
//TODO Send state of devices LED panel to MQTT so HA knows when light is on/off


//BUG if a temp request happens at the same time one is being sent, the later one errors, make the error more clear in MQTT and use queue to