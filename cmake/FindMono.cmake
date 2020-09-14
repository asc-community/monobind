find_program(MONO_EXECUTABLE mono)
find_program(MCS_EXECUTABLE mcs)
find_program(GACUTIL_EXECUTABLE gacutil)
find_program(MSBUILD_EXECUTABLE msbuild)

set(MONO_FOUND FALSE CACHE INTERNAL "")

if(MONO_EXECUTABLE AND MCS_EXECUTABLE AND GACUTIL_EXECUTABLE AND MSBUILD_EXECUTABLE)
    set(MONO_FOUND TRUE CACHE INTERNAL "")

    if(APPLE)
        find_path(MONO_ROOT_DIR
                NAMES Mono/mono-2.0/mono/metadata/appdomain.h
                PATHS "/usr;/usr/local;")

        set(MONO_INCLUDE_DIR "${MONO_ROOT_DIR}/Headers/mono-2.0" CACHE PATH "Mono include directory")
        set(MONO_LIBRARIES "${MONO_ROOT_DIR}/Libraries/libmonosgen-2.0.dylib" CACHE PATH "Mono library")
    elseif(WIN32)
        find_path(MONO_ROOT_DIR
                NAMES include/mono-2.0/mono/metadata/appdomain.h
                PATHS "C:/Program Files (x86)/Mono;C:/Program Files/Mono;")

        set(MONO_INCLUDE_DIR "${MONO_ROOT_DIR}/include/mono-2.0" CACHE PATH "Mono include directory")
        set(MONO_LIBRARIES "${MONO_ROOT_DIR}/lib/mono-2.0-sgen.lib" CACHE PATH "Mono library")
    elseif(LINUX)
        find_path(MONO_ROOT_DIR
                NAMES include/mono-2.0/mono/metadata/appdomain.h
                PATHS "/usr;/usr/local;")

        set(MONO_INCLUDE_DIR "${MONO_ROOT_DIR}/include/mono-2.0" CACHE PATH "Mono include directory")
        set(MONO_LIBRARIES "${MONO_ROOT_DIR}/lib/libmonosgen-2.0.a" CACHE PATH "Mono library")
    endif()

    execute_process(COMMAND ${MCS_EXECUTABLE} --version OUTPUT_VARIABLE MONO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE ".*version ([^ ]+)" "\\1" MONO_VERSION "${MONO_VERSION}")
endif()

mark_as_advanced(MONO_EXECUTABLE MCS_EXECUTABLE GACUTIL_EXECUTABLE MSBUILD_EXECUTABLE MONO_VERSION)
