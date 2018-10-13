project(test_geemuboi_input)

add_executable(test_geemuboi_input
    test_input.cpp
)

target_link_libraries(test_geemuboi_input
    PRIVATE
        geemuboi_input
        gtest
        gtest_main
)

target_compile_options(test_geemuboi_input 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

include(GoogleTest)
gtest_discover_tests(test_geemuboi_input)
