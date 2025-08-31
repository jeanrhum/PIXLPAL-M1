#ifndef EXAMPLE_USE_ENCODER_H
#define EXAMPLE_USE_ENCODER_H

// #include <stdio.h>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mtb_colors.h"
// #include "encoder.h"
// #include "button.h"


extern TaskHandle_t exampleEncoderBeepApp_Task_H;
extern void exampleEncoderBeepApp_Task(void *arguments);

#endif