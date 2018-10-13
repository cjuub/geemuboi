project(test_geemuboi_view)

add_executable(test_geemuboi_view
    test_view.cpp
)

target_link_libraries(test_geemuboi_view
    PRIVATE
        geemuboi_view
        gtest
        gtest_main
)

target_compile_options(test_geemuboi_view 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

include(GoogleTest)
gtest_discover_tests(test_geemuboi_view)
