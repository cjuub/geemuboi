project(geemuboi_input)

add_library(geemuboi_input STATIC
    sdl_keyboard.cpp
)

target_compile_options(geemuboi_input
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

