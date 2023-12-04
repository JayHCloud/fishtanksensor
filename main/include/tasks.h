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


void tempTask(void *arg);
void emergencyTask(void *arg);
void toggleTask(void *arg);

#define tempAlarm      (1 << 0)   // Bit 0 for temperature alarm
#define toggleLight    (1 << 1)   // Bit 1 for toggle light signal

/*
toggleTask stack high watermark: 5844
tempTask stack high watermark: 5852
emergencyTask stack high watermark: 5848
*/