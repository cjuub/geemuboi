project(test_geemuboi_core)

add_executable(test_geemuboi_core
    test_cpu.cpp
)

target_link_libraries(test_geemuboi_core
    PRIVATE
        geemuboi_core
        gtest
        gtest_main
)

target_compile_options(test_geemuboi_core 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

include(GoogleTest)
gtest_discover_tests(test_geemuboi_core)
