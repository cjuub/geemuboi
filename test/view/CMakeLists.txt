project(test_geemuboi_view)

add_executable(${PROJECT_NAME}
    test_view.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        geemuboi_view
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

target_compile_features(${PROJECT_NAME} 
    PRIVATE 
        cxx_std_17
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
