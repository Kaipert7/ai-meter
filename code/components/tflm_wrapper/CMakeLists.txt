idf_component_register(
  SRCS ""
  INCLUDE_DIRS
    ../esp-tflite-micro/tensorflow
    ../esp-tflite-micro/third_party
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-shadow
)

target_include_directories(${COMPONENT_LIB} SYSTEM PUBLIC
    ../esp-tflite-micro/tensorflow
    ../esp-tflite-micro/third_party
)
