project(geemuboi_core)

add_library(geemuboi_core STATIC
    cpu_factory.cpp
    cpu.cpp
    gpu.cpp
    input.cpp
    mmu.cpp
)

target_compile_options(geemuboi_core
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(geemuboi_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/geemuboi
)
