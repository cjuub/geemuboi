project(test_geemuboi_utils)

find_package(GTest REQUIRED)

add_executable(test_geemuboi_utils
    test_utils.cpp
)

target_link_libraries(test_geemuboi_utils
    PRIVATE
        geemuboi_utils
        GTest::GTest
        GTest::Main
)

target_compile_options(test_geemuboi_utils 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
