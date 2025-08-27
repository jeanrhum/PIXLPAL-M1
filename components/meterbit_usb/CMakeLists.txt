#set(PREBUILT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/libmeterbit_usb.a")

idf_component_register(
                    SRCS "mtb_usb_msc.cpp" "mtb_usb_fs.cpp" "mtb_usb_ota.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES usb espressif__usb_host_msc fatfs vfs driver esp_timer  meterbit_system arduino-esp32 meterbit_engine espressif__esp_msc_ota
                    )


# Add the static library file to the component
#target_link_libraries(${COMPONENT_LIB} INTERFACE "${PREBUILT_LIB}")