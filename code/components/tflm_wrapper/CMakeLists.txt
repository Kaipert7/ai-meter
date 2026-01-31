idf_component_register(
  SRCS ""   # keine Quellen -> INTERFACE-Target
  INCLUDE_DIRS
    ../esp-tflite-micro/tensorflow
    ../esp-tflite-micro/third_party
)

# Nur Header als SYSTEM exportieren (unterdrückt Warnungen aus diesen Headerpfaden)
target_include_directories(${COMPONENT_LIB} SYSTEM INTERFACE
  ../esp-tflite-micro/tensorflow
  ../esp-tflite-micro/third_party
)

# WICHTIG: Keine PRIVATE/PUBLIC hier verwenden, weil es ein INTERFACE-Target ist.
# Und: target_compile_options(...) weglassen, damit -Wno-shadow NICHT auf Konsumenten übergeht.
