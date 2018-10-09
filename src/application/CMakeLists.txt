project(geemuboi_application)

add_executable(geemuboi_application
    argument_handler.cpp
    geemuboi.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(geemuboi_application
    PRIVATE
        geemuboi_core
        geemuboi_view
        geemuboi_utils
        geemuboi_input
        SDL2::SDL2
)

target_compile_options(geemuboi_application 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
