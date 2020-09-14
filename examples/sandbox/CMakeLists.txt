add_executable(sandbox sandbox.cpp)

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy ${CMAKE_CURRENT_SOURCE_DIR}/"Dog.cs" "${CMAKE_CURRENT_BINARY_DIR}"
)

target_include_directories(sandbox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)