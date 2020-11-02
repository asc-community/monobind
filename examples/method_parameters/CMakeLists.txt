add_executable(method_parameters method_parameters.cpp)

add_custom_command(TARGET method_parameters POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/"MethodParameters.cs" "${CMAKE_CURRENT_BINARY_DIR}"
)
add_mono_dll_copy_command(method_parameters)

target_include_directories(method_parameters PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)