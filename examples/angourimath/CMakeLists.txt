add_executable(angourimath angourimath.cpp)

set(DLL_LIST 
    "AngouriMath.dll" 
    "Numbers.dll" 
    "GenericTensor.dll" 
    "Antlr4.Runtime.Standard.dll"
)

foreach(CURRENT_DLL IN LISTS DLL_LIST)
    add_custom_command(TARGET angourimath POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy ${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_DLL} "${CMAKE_CURRENT_BINARY_DIR}"
    )
endforeach()

target_include_directories(angourimath PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)