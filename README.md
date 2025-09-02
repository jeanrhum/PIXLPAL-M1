# PIXLPAL – A Project by [Meterbit Cybernetics](https://meterbitcyb.com)

## Overview
Pixlpal is a smart AIoT desktop companion featuring an interactive 128×64 RGB LED matrix display.  
The **Pixlpal-M1** firmware is built on the **ESP-IDF** framework, combining ESP-IDF components, Arduino libraries, and custom applications for the ESP32-S3 SoC.  

Each application runs as a standalone **FreeRTOS** task and can be launched via BLE commands from the Pixlpal Android or iOS mobile app.  
The firmware demonstrates how to build network-enabled visual applications such as clocks, calendars, stock/crypto tickers, AI-powered conversational assistants, and various API integrations.

<p align="center">
  <img src="https://github.com/Meterbit/PIXLPAL-M1/blob/2076007f27073ba415204921fb2bb9618e2c804c/Pixpal-Github.jpeg" alt="Pixlpal Cover Image">
</p>

---

## Key Features
- **Modular Application Framework** – Built on FreeRTOS tasks for easy expansion.
- **ESP32-S3 Target** – Optimized for neural network acceleration and signal processing.
- **Flexible Library Integration** – Supports any Arduino library or ESP-IDF component.
- **Rich Graphics Capabilities** – Draw text, shapes, GIF, PNG, and SVG images.
- **Connectivity** – Wi-Fi for internet access, BLE for remote control and configuration.
- **Audio Processing** – Microphone input and DAC audio output via I2S (`ESP32-audioI2S` by schreibfaul1).
- **USB-OTG/CDC** – Read/write USB flash drives.
- **Dual OTA Update Modes**:  
  1. From GitHub releases  
  2. From USB flash drive (offline)
- **Rotary Encoder Control** – Multi-functional input (`ebtn` by saawsm).
- **MQTT Support** – Integrated via the `PicoMQTT` library by Mlesniew.

---

## Limitations
- **Requires PSRAM** – Tested on the ESP32-S3 (N16R8: 16 MB flash, 8 MB PSRAM).
- **No Bluetooth Classic** – BLE 5.0 only; use an external Bluetooth transmitter for wireless audio.
- **No Battery Management** – No onboard battery system.

---

## Planned Features / To-Do
- Vertical text scrolling.
- JPEG/JPG decoder implementation.
- Integration of `esp-nn` (Espressif’s official neural network library).
- External speaker and mouse support.
- Completion of in-progress apps:  
  iOS Notifications, Google & Outlook Calendars, News RSS, OpenWeather, and World Clock.
- BLE Central mode for wireless sensor integration.
- Addition of BOM to hardware design files.

---

## Quick Start

**Build Requirements:**
- **ESP-IDF v5.3.2** installed on macOS or Windows (via VS Code).  
  - [ESP-IDF VS Code Installation Guide](https://github.com/espressif/vscode-esp-idf-extension)  
  - [Windows Installation Video](https://www.youtube.com/watch?v=D0fRc4XHBNk)  

**Steps:**
1. Clone or download this repository.
2. In VS Code: **File → Open Folder** → select the Pixlpal-M1 project folder.
3. Build the project by clicking the **build icon (spanner)** in the bottom status bar.

---

## Main Program Entry Point
**`main.cpp` – Application Startup Flow**
```c++
#include "Arduino.h"
#include "mtb_system.h"
#include "mtb_engine.h"
#include "mtb_wifi.h"
#include "mtb_ota.h"
#include "mtb_littleFs.h"
#include "mtb_ble.h"
#include "esp_heap_caps.h"
#include "exampleEncoderBeepApp.h"
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
    // mtb_General_App_Lunch(currentApp);
    mtb_Launch_This_App(exampleWriteText_App);

    // Declare Variable for monitoring Free/Available internal SRAM
    size_t free_sram = 0;

    // While Loop prints available Internal SRAM every 2 seconds
    while (1){
    // Get the total free size of internal SRAM
    free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);

    // Print the free SRAM size
    ESP_LOGI(TAG, "############ Free Internal SRAM: %zu bytes\n", free_sram);
    //ESP_LOGI(TAG, "Memory: Free %dKiB Low: %dKiB\n", (int)xPortGetFreeHeapSize()/1024, (int)xPortGetMinimumEverFreeHeapSize()/1024);

    // delay 2 seconds
    delay(2000);
     }

}

```

### Example Application
**`exampleWriteTextApp.cpp` – Displaying Fixed & Scrolling Text**
````c++
#include <Arduino.h>
#include "mtb_text_scroll.h"
#include "mtb_engine.h"
#include "exampleWriteTextApp.h"

EXT_RAM_BSS_ATTR TaskHandle_t exampleWriteTextApp_Task_H = NULL;

EXT_RAM_BSS_ATTR Mtb_Applications_FullScreen *exampleWriteText_App = new Mtb_Applications_FullScreen(exampleWriteTextApp_Task, &exampleWriteTextApp_Task_H, "exampleWriteTextApp", 4096);

void exampleWriteTextApp_Task(void* dApplication){
// ****** Initialize the App Parameters
  Mtb_Applications *thisApp = (Mtb_Applications *)dApplication;
  //mtb_Ble_AppComm_Parser_Sv->mtb_Register_Ble_Comm_ServiceFns(exampleAppBleComTest);
  mtb_App_Init(thisApp);
// End of App parameter initialization


// Declare Fixed and Scroll Text Objects
Mtb_FixedText_t exampleFixedText(24,15, Terminal8x12, GREEN);
Mtb_ScrollText_t exampleScrollText (5, 40, 118, WHITE, 20, 1, Terminal6x8);

// Write Fixed Text to display
exampleFixedText.mtb_Write_String("Hello World."); // Write text in default color
//exampleFixedText.mtb_Write_Colored_String(" in Color!", PURPLE);     // Write text in different color
//dma_display->write("Test the text writing function");  // Write text without clearing previous text

while (MTB_APP_IS_ACTIVE == pdTRUE) {
// Scroll the ScrollText Variable on display every 15 seconds
exampleScrollText.mtb_Scroll_This_Text("PIXLPAL - A project by Meterbit Cybernetics");      // Scroll text in default color
//exampleScrollText.mtb_Scroll_This_Text("Visit us at www.meterbitcyb.com", CYAN);          // Scroll text in different color
delay(15000);
}

// Clean up the application before exiting
  mtb_End_This_App(thisApp);
}
````

## Control Circuit Schematic
<p align="center">
  <img src="https://github.com/Meterbit/PIXLPAL-M1/blob/af65b201861f3fa28d05fecc5947d0f03bc81e3b/Pixlpal%20Schematic%20-%20BOM%20-%20Enclosure%20files/Pixlpal-M1%20Schematic.png" alt="Pixlpal Control Schematic">
</p>

## Licenses

- Hardware Design Licensed under the [CERN-OHL-W](https://gitlab.com/ohwr/project/cernohl/-/wikis/uploads/f773df342791cc55b35ac4f907c78602/cern_ohl_w_v2.pdf)
- Software/Firmware Design Licensed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html?utm_source=chatgpt.com#license-text)
