cmake_minimum_required(VERSION 3.16)
project(nerveBall LANGUAGES CXX)

##set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/) #output the thing in the same directory as the source
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

## ADD executable and additional sources
add_executable(nerveBall ./nerveBall.cpp ./synth.cpp)

target_link_libraries(nerveBall PRIVATE sfml-graphics sfml-audio sfml-window sfml-system) 
target_compile_features(nerveBall PRIVATE cxx_std_17) 


## Dunno about this...
if(WIN32)
    add_custom_command(
        TARGET nerveBall
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:nerveBall>
        VERBATIM)
endif()

install(TARGETS nerveBall)
