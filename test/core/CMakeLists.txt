project(test_geemuboi_core)

add_executable(test_geemuboi_core
    test_cpu.cpp
)

target_link_libraries(test_geemuboi_core
    PRIVATE
        geemuboi_core
        geemuboi_utils
        gtest
        gtest_main
        gmock
)

target_compile_options(test_geemuboi_core 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

target_include_directories(test_geemuboi_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/test
)

include(GoogleTest)
gtest_discover_tests(test_geemuboi_core)
