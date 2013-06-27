project(opencv_logger)

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_BUILD_TYPE Debug)
#add_definitions(-O0)
#add_definitions(-g3)

set(CMAKE_BUILD_TYPE Release)
add_definitions(-O3)

find_package(OpenCV REQUIRED)

include_directories (
  include
)

set(LIBRARY_OUTPUT_PATH lib)

add_library(opencv_logger SHARED src/Logger.cpp)

target_link_libraries(opencv_logger ${OpenCV_LIBS})

install (
  TARGETS opencv_logger
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
)

FILE(GLOB header_files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
install (
  FILES ${header_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/opencv_logger/
)
