#include <Arduino.h>
#include <HTTPClient.h>
#include "mtb_github.h"
#include "mtb_text_scroll.h"
#include <time.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "mtb_graphics.h"
#include "mtb_nvs.h"
#include "mtb_engine.h"
#include "studioLight.h"
#include "mtb_buzzer.h"

StudioLight_Data_t studioLightsInfo;
EXT_RAM_BSS_ATTR SemaphoreHandle_t studioLightMode_Sem_H = NULL;

EXT_RAM_BSS_ATTR TaskHandle_t studioLight_Task_H = NULL;
void studioLight_App_Task(void *);
// supporting functions

// button and encoder functions
void selectStudioLightColorButton(button_event_t button_Data);

// bluetooth functions
void setStudioLightColors(JsonDocument&);
void setStudioLightMode(JsonDocument&);
void setStudioLightDuration(JsonDocument&);

EXT_RAM_BSS_ATTR Mtb_Applications_FullScreen *studioLight_App = new Mtb_Applications_FullScreen(studioLight_App_Task, &studioLight_Task_H, "studioLight", 4096);

void studioLight_App_Task(void* dApplication){
  Mtb_Applications *thisApp = (Mtb_Applications *)dApplication;
  thisApp->mtb_App_EncoderFn_ptr = mtb_Brightness_Control;
  thisApp->mtb_App_ButtonFn_ptr = selectStudioLightColorButton;
  mtb_Ble_AppComm_Parser_Sv->mtb_Register_Ble_Comm_ServiceFns(setStudioLightColors, setStudioLightMode, setStudioLightDuration);
  mtb_App_Init(thisApp);
  //************************************************************************************ */
    if(studioLightMode_Sem_H == NULL) studioLightMode_Sem_H = xSemaphoreCreateBinary();


  mtb_Read_Nvs_Struct("studioLight", &studioLightsInfo, sizeof(StudioLight_Data_t));

    while (MTB_APP_IS_ACTIVE == pdTRUE){
    if(xSemaphoreTake(studioLightMode_Sem_H, pdMS_TO_TICKS(100)) == pdTRUE){
            if(studioLightsInfo.studioLightColorMode == FULLSCREEN_MODE){
                dma_display->fillScreen(studioLightsInfo.studioLightColor[0]);
            }else if (studioLightsInfo.studioLightColorMode == HALFSCREEN_MODE){
                dma_display->fillRect(0, 0, MATRIX_WIDTH/2, MATRIX_HEIGHT, studioLightsInfo.studioLightColor[0]);
                dma_display->fillRect(MATRIX_WIDTH/2, 0, MATRIX_WIDTH/2, MATRIX_HEIGHT, studioLightsInfo.studioLightColor[1]);
            }else if (studioLightsInfo.studioLightColorMode == CYCLE_MODE){
                static uint8_t colorIndex = 0;
                dma_display->fillScreen(studioLightsInfo.studioLightColor[colorIndex]);
                colorIndex++;
                if(colorIndex >= 10) colorIndex = 0;
            }
        }
    }
    vSemaphoreDelete(studioLightMode_Sem_H);
    studioLightMode_Sem_H = NULL;
  mtb_End_This_App(thisApp);
}

void selectStudioLightColorButton(button_event_t button_Data){
            switch (button_Data.type){
            case BUTTON_RELEASED:
            break;

            case BUTTON_PRESSED:
            //xSemaphoreGive(studioLightMode_Sem_H);
            do_beep(CLICK_BEEP);
            mtb_Start_This_Service(mtb_Usb_Audio_Sv);
            break;

            case BUTTON_PRESSED_LONG:
            break;

            case BUTTON_CLICKED:
            //ESP_LOGI(TAG, "Button Clicked: %d Times\n",button_Data.count);
            switch (button_Data.count){
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            default:
                break;
            }
                break;
            default:
            break;
			}
}

void setStudioLightColors(JsonDocument& dCommand){
    uint8_t cmdNumber = dCommand["app_command"];
    String location = dCommand["duration"];

    mtb_Write_Nvs_Struct("studioLight", &studioLightsInfo, sizeof(StudioLight_Data_t));
    mtb_Ble_App_Cmd_Respond_Success(studioLightAppRoute, cmdNumber, pdPASS);
}

void setStudioLightMode(JsonDocument& dCommand){
    uint8_t cmdNumber = dCommand["app_command"];
    uint8_t mode = dCommand["lightMode"];
    if(mode <= CYCLE_MODE) studioLightsInfo.studioLightColorMode = mode;
    xSemaphoreGive(studioLightMode_Sem_H);
    mtb_Write_Nvs_Struct("studioLight", &studioLightsInfo, sizeof(StudioLight_Data_t));
    mtb_Ble_App_Cmd_Respond_Success(studioLightAppRoute, cmdNumber, pdPASS);
}

void setStudioLightDuration(JsonDocument& dCommand){
    uint8_t cmdNumber = dCommand["app_command"];
    uint16_t duration = dCommand["duration"];
    studioLightsInfo.studioLightDuration = duration;
    mtb_Write_Nvs_Struct("studioLight", &studioLightsInfo, sizeof(StudioLight_Data_t));
    mtb_Ble_App_Cmd_Respond_Success(studioLightAppRoute, cmdNumber, pdPASS);
}