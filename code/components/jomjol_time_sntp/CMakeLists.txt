FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

idf_component_register(SRCS "time_sntp.cpp"
                      INCLUDE_DIRS "."
                      REQUIRES esp_event lwip jomjol_logfile)


