add_executable(simple_function_call simple_function_call.cpp)

add_custom_command(TARGET simple_function_call POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/"SimpleFunctionCall.cs" "${CMAKE_CURRENT_BINARY_DIR}"
)

target_include_directories(simple_function_call PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)