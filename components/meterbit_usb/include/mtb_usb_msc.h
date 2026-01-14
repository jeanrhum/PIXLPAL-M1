
#ifndef USB_MSC_H
#define USB_MSC_H

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif
#define USB_MOUNTED_BIT BIT0
//extern void file_operations(void);
extern TaskHandle_t usb_Mass_Storage_H;

#ifdef __cplusplus
}
#endif

#endif