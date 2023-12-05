#pragma once

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include "include/mqtt.h"
#include "include/ledconfig.h"
#include "include/watertemp.h"


extern EventGroupHandle_t tasksGroup;

void getTempGap(void);

void tasksInit();
void tempTask(void *arg);
void apiTask(void *arg);


#define tempAlarm          (1 << 0)   // Bit 0 for temperature alarm
#define toggleLight        (1 << 1)   // Bit 1 for toggle light signal
#define takeTemperature    (1 << 2)   // Bit 2 for mqtt tempature request
