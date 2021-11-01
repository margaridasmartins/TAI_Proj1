cmake_minimum_required(VERSION 3.0.0)
project(fcm VERSION 0.1.0)

include(FetchContent) # If not included already

FetchContent_Declare(sciplot_content
  GIT_REPOSITORY https://github.com/sciplot/sciplot.git
  GIT_TAG master)

FetchContent_GetProperties(sciplot_content)
if(NOT sciplot_content_POPULATED)
  FetchContent_Populate(sciplot_content)
endif()

include_directories(${sciplot_content_SOURCE_DIR})

add_executable(fcm fcm.cpp)
add_executable(generator generator.cpp)

set_target_properties(fcm PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")
set_target_properties(generator PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

target_compile_features(fcm PUBLIC cxx_std_17) # use c++17 when compiling fcm
set_target_properties(fcm PROPERTIES CXX_EXTENSIONS OFF) # don't use gnu extensions
