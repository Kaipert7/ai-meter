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
    # -> mdns wird NUR hier im C++-Code genutzt (connect_wlan.cpp),
    #    deshalb als PRIV_REQUIRES, damit die Header/Libs verfügbar sind.
    #
    PRIV_REQUIRES
        mdns

    #
    # Öffentliche (propagierte) Abhängigkeiten:
    # -> WLAN/Netif/Event/NVS sind für die STA-Initialisierung notwendig.
    # -> http_client wird im WLAN-Code häufig für Tests/Aufrufe genutzt;
    #    falls in deinem connect_wlan.cpp NICHT verwendet, kannst du es entfernen.
    #
    REQUIRES
        esp_wifi
        esp_event
        esp_netif
        nvs_flash
        esp_http_client
)
