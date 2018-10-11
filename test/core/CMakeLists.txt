project(test_geemuboi_core)

find_package(GTest REQUIRED)

add_executable(test_geemuboi_core
    test_cpu.cpp
)

target_link_libraries(test_geemuboi_core
    PRIVATE
        geemuboi_core
        GTest::GTest
        GTest::Main
)

target_compile_options(test_geemuboi_core 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
