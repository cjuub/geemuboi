project(test_geemuboi_view)

find_package(GTest REQUIRED)

add_executable(test_geemuboi_view
    test_view.cpp
)

target_link_libraries(test_geemuboi_view
    PRIVATE
        geemuboi_view
        GTest::GTest
        GTest::Main
)

target_compile_options(test_geemuboi_view 
    PRIVATE 
        -Wall
        -Wextra
        -pedantic-errors
        -Wold-style-cast
)
