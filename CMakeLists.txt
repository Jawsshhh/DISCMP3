# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#

# Add source to this project's executable.

cmake_minimum_required(VERSION 3.10)
project("STDISCM-SFML-CMAKE")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CACHE PATH "Base directory of libs" FORCE)
set(RESOURCES_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Media CACHE PATH "Base directory of resources" FORCE)
set(PROJ_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Project SRC" FORCE)

file(GLOB_RECURSE PROJ_SRC CONFIGURE_DEPENDS "${PROJ_SRC_PATH}/*.[ch]pp")

# Fix for OpenAL issue - disable audio if you don't need it
#set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)

# set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)
# set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(STDISCM-SFML-CMAKE "${PROJ_SRC}")
target_compile_features(STDISCM-SFML-CMAKE PRIVATE cxx_std_20)

target_link_libraries(STDISCM-SFML-CMAKE PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Fixed typo: CNAKE -> CMAKE
add_custom_command(
    TARGET STDISCM-SFML-CMAKE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Media
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Media
)

# Copy OpenAL DLL on Windows (if audio is enabled)
# Copy OpenAL DLL on Windows
if(WIN32)
    add_custom_command(
        TARGET STDISCM-SFML-CMAKE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FETCHCONTENT_BASE_DIR}/sfml-src/extlibs/bin/x64/openal32.dll
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/openal32.dll
    )
endif()