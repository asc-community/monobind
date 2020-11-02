add_executable(custom_types custom_types.cpp)

add_custom_command(TARGET custom_types POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/"CustomTypes.cs" "${CMAKE_CURRENT_BINARY_DIR}"
)
add_mono_dll_copy_command(custom_types)
target_include_directories(custom_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)