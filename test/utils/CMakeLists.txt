project(test_geemuboi_utils)

add_executable(test_geemuboi_utils
    test_utils.cpp
)

target_link_libraries(test_geemuboi_utils
    PRIVATE
        geemuboi_utils
        gtest
        gtest_main
)

target_compile_options(test_geemuboi_utils 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

include(GoogleTest)
gtest_discover_tests(test_geemuboi_utils)
