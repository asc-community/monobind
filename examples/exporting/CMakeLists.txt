add_executable(exporting exporting.cpp)
add_mono_dll_copy_command(exporting)
target_include_directories(exporting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)