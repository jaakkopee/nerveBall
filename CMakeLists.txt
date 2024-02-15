cmake_minimum_required(VERSION 3.0)

project(nerveBall)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/) #output the thing in the same directory as the source

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

## ADD executable and additional sources
add_executable(nerveBall ./nerveBall.cpp ./synth.cpp)

## ADD definitions
target_compile_definitions(nerveBall PRIVATE SFML_STATIC)

## ADD libraries
target_link_libraries(nerveBall PRIVATE sfml-graphics sfml-audio sfml-window sfml-system X11)

install(TARGETS nerveBall)