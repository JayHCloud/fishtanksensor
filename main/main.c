
#include "include/mqtt.h"
#include "include/tasks.h"

#define TASK_STACK_SIZE 16000

void app_main(void)
{

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_sta(); 
    mqtt_app_start(); 
    configure_led();  



    tasksGroup = xEventGroupCreate();

    xTaskCreatePinnedToCore(tempTask, "TempTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(emergencyTask, "EmergencyTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(toggleTask, "ToggleTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);






}


//TODO Make Generic LED func that takes mqtt? input and changes to colour based on RBG values given
//TODO Add level sensors to tell me when water needs to be added, on 2nd thought water evaporates pretty consistently and maybe i could just do some math
//TODO Look into PH sensor