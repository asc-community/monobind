add_executable(virtual_methods virtual_methods.cpp)

add_custom_command(TARGET virtual_methods POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/"VirtualMethods.cs" "${CMAKE_CURRENT_BINARY_DIR}"
)

target_include_directories(virtual_methods PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)