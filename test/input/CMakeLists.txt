project(test_geemuboi_input)

find_package(GTest REQUIRED)

add_executable(test_geemuboi_input
    test_input.cpp
)

target_link_libraries(test_geemuboi_input
    PRIVATE
        geemuboi_input
        GTest::GTest
        GTest::Main
)

target_compile_options(test_geemuboi_input 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
