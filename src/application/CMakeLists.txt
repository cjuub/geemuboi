project(geemuboi_application)

add_executable(${PROJECT_NAME}
    geemuboi.cpp
)

find_package(SDL2 REQUIRED)

include(FetchContent)
FetchContent_Declare(
    args
    GIT_REPOSITORY https://github.com/Taywee/args
)

FetchContent_GetProperties(args)
if(NOT args_POPULATED)
  FetchContent_Populate(args)
  add_subdirectory(${args_SOURCE_DIR} ${args_BINARY_DIR})
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        geemuboi_core
        geemuboi_view
        geemuboi_input
        SDL2::SDL2
        args
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
