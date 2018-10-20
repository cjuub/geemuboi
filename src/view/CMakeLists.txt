project(geemuboi_view)

add_library(geemuboi_view STATIC
    sdl_renderer.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(geemuboi_view
    PUBLIC
        SDL2::SDL2
)

target_compile_options(geemuboi_view
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(geemuboi_view
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
