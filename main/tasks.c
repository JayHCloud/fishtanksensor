#include <include/tasks.h>


TaskHandle_t tempTaskHandle = NULL;
TaskHandle_t emergencyTaskHandle = NULL;
TaskHandle_t toggleTaskHandle = NULL;


static int count = 0;
static float currentTemp = 0;
static float previousTemp = 0;
static float tempGap = 0;

#define TASK_STACK_SIZE 16000

//#define tempAlarm      (1 << 0)    Bit 0 for temperature alarm
//#define toggleLight    (1 << 1)    Bit 1 for toggle light signal


EventGroupHandle_t tasksGroup;


void tasksInit(){

    tasksGroup = xEventGroupCreate();
    xTaskCreatePinnedToCore(tempTask, "TempTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(emergencyTask, "EmergencyTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(toggleTask, "ToggleTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);


}


void getTempGap(){

    if(( currentTemp !=0) && (previousTemp !=0)){ // Would also trigger on error code , may not be ideal
        tempGap = currentTemp - previousTemp;
        tempGap = (tempGap < 0) ? -tempGap : tempGap;
    }

    else{tempGap = 0;
    
    }

}


void tempTask(void *arg)
{

    while(1){
        previousTemp = currentTemp;
        currentTemp = ds18b20_get_temp(); 
        getTempGap();

        if (count >= 20){ //Every 10 Minutes, send the temperature
            ds18b20_send_mqtt();
            count = 0;
        } 

        if (tempGap >= 2.0 ){ //2 degrees difference in 30 seconds

            xEventGroupSetBits(tasksGroup, tempAlarm);
            
        }

        
        if (tempGap < 2.0 ){

            xEventGroupClearBits(tasksGroup, tempAlarm);
            
        }

        count++;
        vTaskDelay(30000 / portTICK_PERIOD_MS); //Block task 30 Sec
        }


    
}


void emergencyTask(void *arg)
{

    while(1){
        EventBits_t bits = xEventGroupWaitBits(tasksGroup, tempAlarm, false, true, portMAX_DELAY);

        if ((bits & tempAlarm) != 0) {
            ledRed();

            while((bits & tempAlarm) != 0){
                bits = xEventGroupWaitBits(tasksGroup, tempAlarm, false, true, portMAX_DELAY);
            }
            ledOff();
        }
    }
}



void toggleTask(void *arg)
{

    while(1){
        EventBits_t bits = xEventGroupWaitBits(tasksGroup, tempAlarm | toggleLight, false, true, portMAX_DELAY);

        if (((bits & tempAlarm) == 0) && ((bits & toggleLight) != 0)) {
            toggle_led();
        }

        xEventGroupClearBits(tasksGroup, toggleLight);
        
    }
}

