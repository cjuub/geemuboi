project(geemuboi_application)

add_executable(geemuboi_application
    geemuboi.cpp
)

find_package(SDL2 REQUIRED)

include(FetchContent)
FetchContent_Declare(
    args
    GIT_REPOSITORY https://github.com/Taywee/args
)

Fetchcontent_GetProperties(args)
if(NOT args_POPULATED)
  FetchContent_Populate(args)
  add_subdirectory(${args_SOURCE_DIR} ${args_BINARY_DIR})
endif()

target_link_libraries(geemuboi_application
    PRIVATE
        geemuboi_core
        geemuboi_view
        geemuboi_utils
        geemuboi_input
        SDL2::SDL2
        args
)

target_compile_options(geemuboi_application 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
