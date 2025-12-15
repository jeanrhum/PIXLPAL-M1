#include "Arduino.h"
#include "mtb_system.h"
#include "mtb_engine.h"
#include "mtb_wifi.h"
#include "mtb_ota.h"
#include "mtb_littleFs.h"
#include "mtb_ble.h"
#include "esp_heap_caps.h"
#include "projdefs.h"

using namespace std;

static const char TAG[] = "PXP-MAIN_PROG";

extern "C" void app_main(){
    // Initialize Pixlpal System
    mtb_LittleFS_Init();
    mtb_RotaryEncoder_Init();
    mtb_System_Init();
    mtb_Ble_Comm_Init();

    // Attempt OTA Update from USB Flash Drive
    mtb_Launch_This_App(usbOTA_Update_App);
    while(Mtb_Applications::firmwareOTA_Status != pdFALSE) delay(1000);

    // Read the last executed App from NVS
    mtb_Read_Nvs_Struct("currentApp", &currentApp, sizeof(Mtb_CurrentApp_t));

    // Initialize Wifi
    mtb_Wifi_Init();

    // Launch the Last Executed App or Launch a particular App after boot-up
    mtb_General_App_Lunch(currentApp);
    // mtb_Launch_This_App(exampleWriteText_App);
    // mtb_Launch_This_App(polygonFX_App);

    // Declare Variable for monitoring Free/Available internal SRAM
    size_t free_sram = 0;

    //printf("The Size of Mtb_ScrollText_t is: %d \n", sizeof(Mtb_ScrollText_t));
    printf("The Size of rotary_encoder_event_t is: %d \n", sizeof(rotary_encoder_event_t));
    printf("The Size of button_event_t is: %d \n", sizeof(button_event_t));
    printf("The Size of NvsAccessParams_t is: %d \n", sizeof(NvsAccessParams_t));
    printf("The Size of Mtb_Applications is: %d \n", sizeof(Mtb_Applications));
    printf("The Size of Mtb_Services is: %d \n", sizeof(Mtb_Services));

    // While Loop prints available Internal SRAM every 2 seconds
    while (1){
        
    // Get the total free size of internal SRAM
    free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);

    // Print the free SRAM size
    printf("############ Free Internal SRAM: %zu bytes\n", free_sram);

    // delay 2 seconds
    delay(5000);
     }
}
