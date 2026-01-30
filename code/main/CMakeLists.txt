# code/main/CMakeLists.txt
# AI-on-the-Edge (ESP32-S3) – Main-Component

# Alle .c/.cpp im Ordner main einsammeln
file(GLOB MAIN_SOURCES
     *.c
     *.cpp
)

idf_component_register(
    SRCS
        ${MAIN_SOURCES}
    INCLUDE_DIRS
        .
    #
    # NUR die Komponenten, deren Header in main/* direkt inkludiert werden:
    #
    REQUIRES
        # --- ESP-IDF Basis für Netzwerk/HTTP/MQTT/PSRAM/Ethernet ---
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
        esp_eth

        # --- Projekt-/Jomjol-Komponenten (laut code/components/) ---
        jomjol_helper
        jomjol_configfile
        jomjol_time_sntp
        jomjol_logfile
        jomjol_controlGPIO    # liefert server_GPIO.h
        jomjol_wlan           # liefert connect_wlan.h
)
