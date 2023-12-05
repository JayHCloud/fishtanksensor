
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
    tasksInit(); //sudden camelcase

}


//TODO Make Generic LED func that takes mqtt? input and changes to colour based on RBG values given
//TODO Add level sensors to tell me when water needs to be added, on 2nd thought water evaporates pretty consistently and maybe i could just do some math
//TODO Look into PH sensor
//TODO Send state of devices LED panel to MQTT so HA knows, maybe implement direct on/off func