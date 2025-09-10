#ifndef EXAMPLE_DRAW_IMAGE_H
#define EXAMPLE_DRAW_IMAGE_H

// #include <stdio.h>
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mtb_graphics.h"

extern TaskHandle_t exampleDrawImageApp_Task_H;
extern void exampleDrawImageApp_Task(void *arguments);

#endif