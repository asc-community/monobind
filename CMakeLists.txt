cmake_minimum_required(VERSION 3.3)

project(monobind VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

SET(MONO_ONLY_LIBRARIES_REQUIRED ON CACHE BOOL "" FORCE)
find_package(Mono REQUIRED)

message(" dirs lib : ${MONO_LIBRARY_DIRS}")

if(MONO_FOUND)
    message("Found Mono ${MONO_VERSION}")
    message("Found exec ${MONO_EXECUTABLE}")
else()
    message(FATAL_ERROR "Could not find Mono!")
endif()

add_compile_definitions(MONOBIND_MONO_ROOT="${MONO_ROOT_DIR}")
include_directories(${MONO_INCLUDE_DIRS})
link_libraries(${MONO_LIBRARIES})
link_directories(${MONO_LIBRARY_DIRS})

option(MONOBIND_ENABLE_EXCEPTIONS "compile with exceptions enabled" TRUE)
option(MONOBIND_BUILD_TESTS "build monobind tests" TRUE)
option(MONOBIND_BUILD_EXAMPLES "build monobind examples" TRUE)

if(NOT MONOBIND_ENABLE_EXCEPTIONS)
    add_compile_definitions(MONOBIND_DISABLE_EXCEPTIONS)
endif()

if (MONOBIND_BUILD_EXAMPLES)
    add_subdirectory(examples/sandbox)
    add_subdirectory(examples/simple_function_call)
    add_subdirectory(examples/method_parameters)
    add_subdirectory(examples/custom_types)
    add_subdirectory(examples/angourimath)
    add_subdirectory(examples/exporting)
endif()