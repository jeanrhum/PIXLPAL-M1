#include <Arduino.h>
#include <HTTPClient.h>
#include "mtb_github.h"
#include "mtb_text_scroll.h"
#include <time.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "mtb_nvs.h"
#include "mtb_engine.h"
#include "workerWorldFlagsFns.h"
#include "worldClock.h"
#include "mtb_buzzer.h"

#define SINGLE_CLOCK_MODE   0
#define FIVE_CLOCK_MODE     1

EXT_RAM_BSS_ATTR TaskHandle_t worldClock_Task_H = NULL;
EXT_RAM_BSS_ATTR WorldClock_Data_t worldClockCities;
//EXT_RAM_BSS_ATTR SemaphoreHandle_t studioLightMode_Sem_H = NULL;
void worldClock_App_Task(void *);
// supporting functions

// button and encoder functions
// void selectAM_PMButton(button_event_t button_Data);

// bluetooth functions
void setWorldClockCities(JsonDocument&);
void setWorldClockColors(JsonDocument&);
void setWorldClockMode(JsonDocument&);
void requestWorldClkNTP_Time(JsonDocument&);

// Helper Functions
void drawWorldClock5CitiesBkgd(void);
void drawWorldClockSingleCity(void);
char* getCityLocalTime(char* cityTimeZone,  char* timeTextBuffer);

// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityName0;
// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityTime0;

// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityName1;
// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityTime1;

// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityName2;
// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityTime2;

// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityName3;
// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityTime3;

// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityName4;
// EXT_RAM_BSS_ATTR Mtb_FixedText_t* cityTime4;

EXT_RAM_BSS_ATTR Mtb_Applications_StatusBar *worldClock_App = new Mtb_Applications_StatusBar(worldClock_App_Task, &worldClock_Task_H, "world Clock", 10240);

void worldClock_App_Task(void* dApplication){
  Mtb_Applications *thisApp = (Mtb_Applications *)dApplication;
  thisApp->mtb_App_EncoderFn_ptr = mtb_Brightness_Control;
  //thisApp->mtb_App_ButtonFn_ptr = selectAM_PMButton;
  mtb_App_BleComm_Parser_Sv->mtb_Register_Ble_Comm_ServiceFns(setWorldClockCities, setWorldClockColors, setWorldClockMode, requestWorldClkNTP_Time);
  mtb_App_Init(thisApp, mtb_Status_Bar_Calendar_Sv);
  //************************************************************************************ */
    //if(studioLightMode_Sem_H == NULL) studioLightMode_Sem_H = xSemaphoreCreateBinary();
    mtb_Read_Nvs_Struct("Clock Cols", &clk_Updt, sizeof(Clock_Colors));
    char rtc_Hr_Min[10] = {0};

    worldClockCities = (WorldClock_Data_t){
    {"New York", "London", "Tokyo", "Sydney", "Moscow"},
    {
    "EST5EDT,M3.2.0/2,M11.1.0/2",
    "GMT0BST,M3.5.0/1,M10.5.0/2",
    "JST-9",
    "AEST-10AEDT,M10.1.0/2,M4.1.0/3",
    "MSK-3"
    },
    "United States of America",
    { GREEN, WHITE, YELLOW, CYAN, MAGENTA },
    0
    };

    Mtb_FixedText_t dispCityTime(70, 21, Terminal10x17, GREEN);
    Mtb_CentreText_t dispCityName(65, 57, Terminal8x12, WHITE);

    Mtb_FixedText_t cityName0(3, 12, Terminal6x8, GREEN_LIZARD);
    Mtb_FixedText_t cityTime0(100, 12, Terminal6x8, WHITE);

    Mtb_FixedText_t cityName1(3, 23, Terminal6x8, GREEN_LIZARD);
    Mtb_FixedText_t cityTime1(100, 23, Terminal6x8, WHITE);

    Mtb_FixedText_t cityName2(3, 34, Terminal6x8, GREEN_LIZARD);
    Mtb_FixedText_t cityTime2(100, 34, Terminal6x8, WHITE);

    Mtb_FixedText_t cityName3(3, 45, Terminal6x8, GREEN_LIZARD);
    Mtb_FixedText_t cityTime3(100, 45, Terminal6x8, WHITE);

    Mtb_FixedText_t cityName4(3, 56, Terminal6x8, GREEN_LIZARD);
    Mtb_FixedText_t cityTime4(100, 56, Terminal6x8, WHITE);

    Mtb_OnlineImage_t worldCountryFlag{
      "https://raw.githubusercontent.com/woble/flags/refs/heads/master/SVG/3x2/ng.svg",
      3,
      14,
      2
    };

  mtb_Read_Nvs_Struct("worldClockNv", &worldClockCities, sizeof(WorldClock_Data_t));
  strcpy(ntp_TimeZone, worldClockCities.worldTimeZones[0]);

if(worldClockCities.worldClockMode == FIVE_CLOCK_MODE) drawWorldClock5CitiesBkgd();
else drawWorldClockSingleCity();

while (MTB_APP_IS_ACTIVE == pdTRUE) {
    thisApp->elementRefresh = false;
    if(worldClockCities.worldClockMode == FIVE_CLOCK_MODE){
      // MEMORY LEAKING OBSERVED IN THIS LOOP - NEEDS FIXING LATER
      cityName0.mtb_Write_Colored_String(worldClockCities.worldCapitals[0], worldClockCities.worldColors[0]);
      cityName1.mtb_Write_Colored_String(worldClockCities.worldCapitals[1], worldClockCities.worldColors[1]);
      cityName2.mtb_Write_Colored_String(worldClockCities.worldCapitals[2], worldClockCities.worldColors[2]);
      cityName3.mtb_Write_Colored_String(worldClockCities.worldCapitals[3], worldClockCities.worldColors[3]);
      cityName4.mtb_Write_Colored_String(worldClockCities.worldCapitals[4], worldClockCities.worldColors[4]);

      while ((Mtb_Applications::internetConnectStatus != true) && (MTB_APP_IS_ACTIVE == pdTRUE)) delay(1000);

      while (MTB_APP_IS_ACTIVE == pdTRUE && thisApp->elementRefresh == false) {
        cityTime0.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[0], rtc_Hr_Min), worldClockCities.worldColors[0]);
        cityTime1.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[1], rtc_Hr_Min), worldClockCities.worldColors[1]);
        cityTime2.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[2], rtc_Hr_Min), worldClockCities.worldColors[2]);
        cityTime3.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[3], rtc_Hr_Min), worldClockCities.worldColors[3]);
        cityTime4.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[4], rtc_Hr_Min), worldClockCities.worldColors[4]);
        delay(1000);
      }
    } else {
      
      dispCityName.mtb_Write_Colored_String(worldClockCities.worldCapitals[0], WHITE);

      while ((Mtb_Applications::internetConnectStatus != true) && (MTB_APP_IS_ACTIVE == pdTRUE)) delay(1000);
      
      strcpy(worldCountryFlag.imageLink, getFlag4x3ByCountry(worldClockCities.firstCountryName).c_str());

      mtb_Draw_Online_Svg(&worldCountryFlag);             // IF FLAG IS NOT DRAWN, IT MEANS THE NAME OF THE COUNTRY WAS NOT FOUND AMONG THE COUNTRIES FLAG LISTS/JSON.

      while (MTB_APP_IS_ACTIVE == pdTRUE && thisApp->elementRefresh == false) {
        dispCityTime.mtb_Write_Colored_String(getCityLocalTime(worldClockCities.worldTimeZones[0], rtc_Hr_Min), worldClockCities.worldColors[0]);
        delay(1000);
      }
    }
}

  mtb_End_This_App(thisApp);
}


void drawWorldClock5CitiesBkgd(void){
    dma_display->fillRect(0, 10, 128, 54, BLACK);   
    //dma_display->fillRect(0, 10, 128, 10, TURTLE_GREEN);
    uint16_t clockDividerColor = dma_display->color565(35, 35, 35);
    dma_display->drawFastHLine(0, 20, 128, clockDividerColor);
    dma_display->drawFastHLine(0, 31, 128, clockDividerColor);
    dma_display->drawFastHLine(0, 42, 128, clockDividerColor);
    dma_display->drawFastHLine(0, 53, 128, clockDividerColor);
}

void drawWorldClockSingleCity(void){
    dma_display->fillRect(0, 10, 128, 54, BLACK);
    mtb_Draw_Local_Png({"/batIcons/worldClk.png", 59, 14});
}

char* getCityLocalTime(char* cityTimeZone, char* timeTextBuffer){
    setenv("TZ", cityTimeZone, 1);
    tzset();

    time_t present = 0;
    struct tm *now = nullptr;
    //    char AM_or_PM;
    static uint8_t pre_Hr = 111;
    uint8_t pre_Min = 111;

    time(&present);
    now = localtime(&present);

  if (pre_Hr != now->tm_hour){
	pre_Hr = now->tm_hour;

    if(pre_Hr == 0){
    timeTextBuffer[0] = '0';
    timeTextBuffer[1] = '0';
    //sprintf(timeTextBuffer, "%d", pre_Hr );
    } else if (pre_Hr < 10){
    timeTextBuffer[0] = '0';
    sprintf(&timeTextBuffer[1], "%d", pre_Hr);
    } else {
        sprintf(timeTextBuffer, "%d", pre_Hr );
	}	
    pre_Hr = now->tm_hour;        // Code is placed here because pre_Hr was changed.
  }

  	if (pre_Min != now->tm_min){
  pre_Min = now->tm_min;

  if (pre_Min < 10){
		timeTextBuffer[3] = '0';
    sprintf(&timeTextBuffer[4], "%d", pre_Min);
		} else {
    sprintf(&timeTextBuffer[3], "%d", pre_Min);
	}
  timeTextBuffer[2] = ':';
  timeTextBuffer[5] = 0;
}
return timeTextBuffer;
}

void setWorldClockCities(JsonDocument& dCommand){
  uint8_t cmdNumber = dCommand["app_command"];
  uint8_t dCityIndex = dCommand["dCityIndex"].as<uint8_t>();
  String dCityName = dCommand["dCityName"].as<String>();
  String dTimeZone = dCommand["dTimeZone"].as<String>();

  dCityName.replace("[", "");
  dCityName.replace("]", "");

  dTimeZone.replace("[", "");
  dTimeZone.replace("]", "");

  int commaIndex = dCityName.indexOf(',');

  if (commaIndex != -1 && dCityIndex == 0) {
    String country = "";
    country = dCityName.substring(commaIndex + 1); // get text after the comma
    country.trim();  // remove leading/trailing spaces
    strcpy(worldClockCities.firstCountryName, country.c_str());
  }

  if (commaIndex != -1) {
    dCityName.remove(commaIndex);   // Removes from the comma to the end
  }

  strcpy(worldClockCities.worldCapitals[dCityIndex], dCityName.c_str());
  strcpy(worldClockCities.worldTimeZones[dCityIndex], dTimeZone.c_str());

  mtb_Write_Nvs_Struct("worldClockNv", &worldClockCities, sizeof(WorldClock_Data_t));
  strcpy(ntp_TimeZone, worldClockCities.worldTimeZones[0]);

  Mtb_Applications::currentRunningApp->elementRefresh = true;
  mtb_Ble_App_Cmd_Respond_Success(worldClockAppRoute, cmdNumber, pdPASS);
}

void setWorldClockColors(JsonDocument& dCommand){
    const char *color = NULL;
    uint8_t cmdNumber = dCommand["app_command"];
    uint8_t dCityIndex = dCommand["dCityIndex"].as<uint8_t>();

    color = dCommand["value"];
    color += 4;

    worldClockCities.worldColors[dCityIndex] = dma_display->color565(((uint8_t)((strtol(color,NULL,16) >> 16))), ((uint8_t)((strtol(color,NULL,16) >> 8))),((uint8_t)((strtol(color,NULL,16) >> 0))));
    
    mtb_Write_Nvs_Struct("worldClockNv", &worldClockCities, sizeof(WorldClock_Data_t));
    Mtb_Applications::currentRunningApp->elementRefresh = true;
    mtb_Ble_App_Cmd_Respond_Success(worldClockAppRoute, cmdNumber, pdPASS);
}

void setWorldClockMode(JsonDocument&){
    uint8_t cmdNumber = dCommand["app_command"];
    worldClockCities.worldClockMode = dCommand["ClockMode"].as<uint8_t>();

    if(worldClockCities.worldClockMode == FIVE_CLOCK_MODE) drawWorldClock5CitiesBkgd();
    else drawWorldClockSingleCity();

    mtb_Write_Nvs_Struct("worldClockNv", &worldClockCities, sizeof(WorldClock_Data_t));
    Mtb_Applications::currentRunningApp->elementRefresh = true;
    mtb_Ble_App_Cmd_Respond_Success(worldClockAppRoute, cmdNumber, pdPASS);
}

void requestWorldClkNTP_Time(JsonDocument&){
    uint8_t cmdNumber = dCommand["app_command"];
    //String location = dCommand["duration"];

    Mtb_Applications::currentRunningApp->elementRefresh = true;
    mtb_Ble_App_Cmd_Respond_Success(worldClockAppRoute, cmdNumber, pdPASS);
}