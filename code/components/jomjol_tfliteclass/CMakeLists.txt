idf_component_register(SRCS tflite_class.cpp
                    INCLUDE_DIRS "."
                    REQUIRES tflm_wrapper esp-nn jomjol_image_proc jomjol_logfile jomjol_flowcontroll jomjol_helper)
