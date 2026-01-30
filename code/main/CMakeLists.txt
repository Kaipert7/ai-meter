# code/main/CMakeLists.txt
# AI-on-the-Edge (ESP32-S3) – Main-Component

# Alle .c/.cpp im Ordner main einsammeln (praktisch für CI)
file(GLOB MAIN_SOURCES
     *.c
     *.cpp
)

idf_component_register(
    SRCS
        ${MAIN_SOURCES}
    INCLUDE_DIRS
        .
    # WICHTIG: Benötigte IDF-Komponenten angeben, damit Header & Libs eingebunden werden
    REQUIRES
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_pm
        driver
        esp_http_server
        esp_http_client
        mdns
        mqtt
        esp_psram
)
