#include <include/tasks.h>
//#define tempAlarm          (1 << 0)    Bit 0 for temperature alarm
//#define toggleLight        (1 << 1)    Bit 1 for toggle light signal
//#define takeTemperature    (1 << 2)    Bit 1 for toggle light signal


TaskHandle_t tempTaskHandle = NULL;
TaskHandle_t emergencyTaskHandle = NULL;
TaskHandle_t apiTaskHandle = NULL;


static int count = 0;
static float currentTemp = 0;
static float previousTemp = 0;
static float tempGap = 0;

#define TASK_STACK_SIZE 16000 // Arbitrarily bigger than the stack high watermark of 7k i measured. 


EventGroupHandle_t tasksGroup;


void tasksInit(){

    tasksGroup = xEventGroupCreate();
    xTaskCreatePinnedToCore(tempTask, "TempTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(apiTask, "apiTask", TASK_STACK_SIZE, NULL, 1, NULL, 1);


}


void getTempGap(){

    if(( currentTemp !=0) && (previousTemp !=0)){ // Would also trigger on error code , may not be ideal
        tempGap = currentTemp - previousTemp;
        tempGap = (tempGap < 0) ? -tempGap : tempGap;
    }

    else{tempGap = 0;
    
    }

}


void tempTask(void *arg) {      //Will only be red for 30sec before the next reading normalizes it. So only good for quick +-2 water tests unless i keep a larger/wider pool of #'s to get an average
    while (1) {
        printf("tempTask: Start\n");//DELETE ME FOR TESTING

        previousTemp = currentTemp;
        currentTemp = ds18b20_get_temp();
        getTempGap();
        ds18b20_send_mqtt(); //DELETE ME FOR TESTING

        printf("tempTask: Temperature Read\n");//DELETE ME FOR TESTING

        if (count >= 20) {
            printf("tempTask: Sending Temperature Data\n");//DELETE ME FOR TESTING
            ds18b20_send_mqtt();
            count = 0;
        }

        if (tempGap >= 2) { //Sets alarm bit and turns led red
            xEventGroupSetBits(tasksGroup, tempAlarm);
            ledRed();
            printf("emergencyTask: Emergency light on\n"); //DELETE ME FOR TESTING
        }

        if (tempGap < 2) {  //Unsets alarm bit and turns led off
            ledOff();
            printf("emergencyTask: Emergency light off, tempAlarm cleared\n"); //DELETE ME FOR TESTING
            xEventGroupClearBits(tasksGroup, tempAlarm);
        }

        count++;

        printf("tempTask: Blocking for 30 Sec\n");//DELETE ME FOR TESTING
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}


void apiTask(void *arg)
{

    while(1){
        EventBits_t bits = xEventGroupWaitBits(tasksGroup, tempAlarm | toggleLight | takeTemperature, false, false, portMAX_DELAY);
        printf("apiTask: Entered api task\n"); //DELETE ME FOR TESTING
        vTaskDelay(3000 / portTICK_PERIOD_MS);  //DELETE ME FOR TESTING

        if (((bits & tempAlarm) == 0) && ((bits & toggleLight) != 0)) {
            //toggle_led();
            printf("apiTask: toggled LED\n"); //DELETE ME FOR TESTING
            xEventGroupClearBits(tasksGroup, toggleLight);
            printf("apiTask: cleared toggle bit\n"); //DELETE ME FOR TESTING
        }

        if (((bits & takeTemperature) != 0)) {
            ds18b20_send_mqtt();
            printf("apiTask: temp sent\n");  //DELETE ME FOR TESTING
            xEventGroupClearBits(tasksGroup, takeTemperature);
            printf("apiTask: cleared takeTemp bit\n"); //DELETE ME FOR TESTING
        }



    }
}

