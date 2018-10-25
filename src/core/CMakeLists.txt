project(geemuboi_core)

add_library(${PROJECT_NAME} STATIC
    cpu_debug_decorator.cpp
    cpu_factory.cpp
    cpu.cpp
    gpu.cpp
    input.cpp
    mmu.cpp
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
