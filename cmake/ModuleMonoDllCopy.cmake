cmake_policy(VERSION 3.16.1)


macro(add_mono_dll_copy_command TARGET_NAME)
if(WIN32)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND  ${CMAKE_COMMAND} 
    -DTTC_SRC_FILE_PATH="${MONO_ROOT_DIR}/bin/mono-2.0-sgen.dll"
    -DTTC_TARGET_FOLDER_PATH="${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_SOURCE_DIR}/cmake/TryToCopy.cmake"
)
endif()
endmacro()

