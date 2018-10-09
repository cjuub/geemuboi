project(geemuboi_view)

add_library(geemuboi_view STATIC
    sdl_renderer.cpp
)

target_compile_options(geemuboi_view
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
