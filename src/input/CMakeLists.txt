project(geemuboi_input)

add_library(${PROJECT_NAME} STATIC
    sdl_keyboard.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        SDL2::SDL2
)

target_compile_options(${PROJECT_NAME}
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/geemuboi
)

target_compile_features(${PROJECT_NAME} 
    PRIVATE 
        cxx_std_17
)
