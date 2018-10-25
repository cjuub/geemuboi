project(test_geemuboi_core)

add_executable(${PROJECT_NAME}
    test_cpu.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        geemuboi_core
        gtest
        gtest_main
        gmock
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
        ${CMAKE_SOURCE_DIR}/include/test
)

target_compile_features(${PROJECT_NAME} 
    PRIVATE 
        cxx_std_17
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
