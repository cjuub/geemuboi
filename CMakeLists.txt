cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

project(geemuboi)

add_subdirectory(src/application)
add_subdirectory(src/core)
add_subdirectory(src/input)
add_subdirectory(src/view)

enable_testing()
add_subdirectory(test)
