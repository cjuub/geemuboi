project(test_geemuboi_utils)

add_executable(${PROJECT_NAME}
    test_utils.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        geemuboi_utils
        gtest
        gtest_main
)

target_compile_options(${PROJECT_NAME} 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
