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
    # WICHTIG: Alle benötigten Komponenten hier benennen.
    # -> IDF bindet Header/Libs nur für deklarierte Komponenten ein.
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

        # --- Projekt-/Jomjol-Komponenten (aus code/components/) ---
        jomjol_helper
        jomjol_configfile
        jomjol_time_sntp
        jomjol_logfile
        jomjol_mqtt
        jomjol_controlGPIO     # liefert i. d. R. server_GPIO.h
        jomjol_wlan            # liefert connect_wlan.h (bei Bedarf Groß-/Kleinschreibung anpassen!)
        jomjol_controlcamera
        jomjol_flowcontroll
        jomjol_image_prc
        jomjol_webhook
        jomjol_fileserver_ota

        # Falls du tflite-Klassen direkt aus main nutzt:
        jomjol_tfliteclass

        # Externe AI/Camera-Komponenten (liegen ebenfalls unter code/components/)
        esp32-camera
        esp-tflite-micro
        esp-nn
        esp-protocols           # enthält u. a. mdns Sub-Komponente
)
