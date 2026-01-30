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
    # WICHTIG: Benötigte IDF-/Projekt-Komponenten deklarieren
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
        esp_eth         # <--- neu: wegen esp_eth.h
        jomjol_helper   # <--- neu: liefert psram.h
)
