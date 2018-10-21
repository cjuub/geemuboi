project(geemuboi_input)

add_library(geemuboi_input STATIC
    sdl_keyboard.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(geemuboi_input
    PUBLIC
        SDL2::SDL2
)

target_compile_options(geemuboi_input
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(geemuboi_input
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/geemuboi
)
