project(geemuboi_core)

add_library(geemuboi_core STATIC
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

