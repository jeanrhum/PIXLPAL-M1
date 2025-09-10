#ifndef EXAMPLE_WRITE_TEXT_H
#define EXAMPLE_WRITE_TEXT_H

// #include <stdio.h>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mtb_colors.h"

extern TaskHandle_t exampleWriteTextApp_Task_H;
extern void exampleWriteTextApp_Task(void *arguments);

#endif