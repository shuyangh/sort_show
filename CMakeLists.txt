cmake_minimum_required(VERSION 3.28)
project(sort_show LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(sort_show src/main.cpp)

target_compile_features(sort_show PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(sort_show PRIVATE -Wall -Wextra)
endif()

target_link_libraries(sort_show PRIVATE sfml-graphics sfml-window sfml-system)

if(WIN32)
    if(EXISTS "${SFML_SOURCE_DIR}/extlibs/bin/x64/openal32.dll")
        add_custom_command(
            TARGET sort_show POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_SOURCE_DIR}/extlibs/bin/x64/openal32.dll"
            $<TARGET_FILE_DIR:sort_show>)
    endif()
endif()