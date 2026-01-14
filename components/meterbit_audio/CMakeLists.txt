idf_component_register(SRCS "mtb_audio.cpp" 
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES usb
                    REQUIRES arduino-esp32 arduinoFFT meterbit_system meterbit_graphics ESP32-audioI2S meterbit_engine espressif__usb_host_uac
                    )
