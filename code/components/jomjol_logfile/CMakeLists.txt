FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.*)


idf_component_register(SRCS "ClassLogFile.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES jomjol_helper)
