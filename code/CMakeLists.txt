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

# esp-tflite-micro gezielt von -Wshadow befreien
if (TARGET __idf_esp-tflite-micro)
  message(STATUS "Apply -Wno-shadow to target: __idf_esp-tflite-micro")
  target_compile_options(__idf_esp-tflite-micro PRIVATE -Wno-shadow)

  # Optional (zusätzlich): die TFLM-Headerpfade als SYSTEM, damit Header-Warnungen stumm sind
  target_include_directories(__idf_esp-tflite-micro SYSTEM PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/components/esp-tflite-micro/tensorflow
    ${CMAKE_CURRENT_LIST_DIR}/components/esp-tflite-micro/third_party
  )
endif()
