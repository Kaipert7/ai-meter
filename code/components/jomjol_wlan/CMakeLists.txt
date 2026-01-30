# code/components/jomjol_wlan/CMakeLists.txt

# Alle C/C++-Quellen in diesem Component einsammeln
file(GLOB WLAN_SOURCES
     *.c
     *.cpp
)

idf_component_register(
    SRCS
        ${WLAN_SOURCES}
    INCLUDE_DIRS
        .
    #
    # Interne (private) Abhängigkeiten dieses Components:
    # mdns wird nur hier im C++-Code (connect_wlan.cpp) genutzt.
    #
    PRIV_REQUIRES
        mdns

    #
    # Öffentliche (propagierte) Abhängigkeiten:
    # - esp_wifi/esp_event/esp_netif/nvs_flash: WLAN/Netz-Init
    # - esp_http_client/esp_http_server: HTTP-Client & -Server (basic_auth.h nutzt esp_http_server.h)
    # - driver: für driver/gpio.h in connect_wlan.cpp
    # - jomjol_helper: liefert Helper.h (read_wlanini.cpp)
    #
    REQUIRES
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_http_client
        esp_http_server
        driver
        jomjol_helper
)
