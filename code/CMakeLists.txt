cmake_minimum_required(VERSION 3.16.0)

list(APPEND EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common components/esp-tflite-micro components/esp-protocols/components/mdns)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
           ${CMAKE_CURRENT_BINARY_DIR}/_version.cpp
    COMMAND ${CMAKE_COMMAND} -P
            ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

if(EXISTS "${SDKCONFIG}.defaults")
    if(EXISTS "sdkconfig.defaults")
        set(SDKCONFIG_DEFAULTS "${SDKCONFIG}.defaults;sdkconfig.defaults")
        message(STATUS "-- Using defaults: ${SDKCONFIG_DEFAULTS} + sdkconfig.defaults")
    else()
        set(SDKCONFIG_DEFAULTS "${SDKCONFIG}.defaults")
    endif()
    message(STATUS "-- Using defaults: ${SDKCONFIG_DEFAULTS}")
endif()

add_compile_definitions(BOARD_S3_PROKYBER_AIE=1)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(AI-on-the-edge)

# Alle von ESP-IDF erstellten Komponententargets holen
idf_build_get_component_targets(ALL_COMPONENT_TARGETS)

# Das Target der TFLite-Micro-Komponente finden und flags setzen
foreach(t ${ALL_COMPONENT_TARGETS})
  # Der Name kann je nach Ordner/Komponente variieren. Häufiges Schema ist "__idf_<komponentenname>".
  if(t STREQUAL "__idf_esp-tflite-micro")
    message(STATUS "Tweak warnings for target: ${t}")
    # 1) Nur hier -Wshadow unterdrücken (betrifft NUR diese Komponente)
    target_compile_options(${t} PRIVATE -Wno-shadow)

    # 2) Alternativ/zusätzlich: die betroffenen Ordner als SYSTEM-Includes markieren,
    #    damit Warnungen aus genau diesen Headerpfaden nicht gezeigt werden:
    target_include_directories(${t} SYSTEM PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/components/esp-tflite-micro/tensorflow
      ${CMAKE_CURRENT_LIST_DIR}/components/esp-tflite-micro/third_party
    )
  endif()
endforeach()
