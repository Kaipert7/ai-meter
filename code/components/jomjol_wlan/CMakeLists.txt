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
    # - mdns wird NUR in diesem Component genutzt (connect_wlan.cpp),
    #   deshalb PRIV_REQUIRES.
    #
    PRIV_REQUIRES
        mdns

    #
    # Öffentliche (propagierte) Abhängigkeiten:
    # - esp_http_server -> NEU: wegen basic_auth.h -> esp_http_server.h
    # - esp_wifi/esp_event/esp_netif/nvs_flash: Basis für WLAN-Init
    # - esp_http_client nur falls im WLAN-Code genutzt (bei dir ja).
    #
    REQUIRES
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_http_client
        esp_http_server    # <--- NEU
)
