project(geemuboi_utils)

add_library(geemuboi_utils STATIC
    logger.cpp
)

target_compile_options(geemuboi_utils
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(geemuboi_utils
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/geemuboi
)
