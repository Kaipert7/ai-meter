idf_component_register(
  SRCS
    time_sntp.cpp
  INCLUDE_DIRS
    "."
  REQUIRES
    jomjol_configfile     # <--- NEU: liefert ConfigFile
    jomjol_logfile
    jomjol_helper
    esp_netif             # (siehe B)
    lwip                  # (siehe B)
)
