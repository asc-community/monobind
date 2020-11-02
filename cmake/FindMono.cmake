# tries to find Mono root folder as well as mono executables
# Defines:
#
# MONO_FOUND             System has Mono dev files, as well as mono, mcs, msbuild and gacutil if not MONO_ONLY_LIBRARIES_REQUIRED
# MONO_EXECUTABLE        Where to find 'mono'
# MCS_EXECUTABLE         Where to find 'mcs'
# MSBUILD_EXECUTABLE     Where to find 'msbuild'
# GACUTIL_EXECUTABLE     Where to find 'gacutil'
# MONO_LIBRARIES         Libraries to link (without full path)
# MONO_ROOT_DIR          Root directory which contains 'lib/', 'etc/' ...
# MONO_LIBRARY_DIRS      Directories containing the libraries
# MONO_INCLUDE_DIRS      The directories containing header files
#
# MONO_VERSION           Version number of the Mono dev libraries
#
# Set MONO_ONLY_LIBRARIES_REQUIRED to proceed searching for mono_root despite the fact that executables weren't found
# 
# License: Zero Clause BSD
# Copyright (C) 2020 MonobindProject
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#


find_program(MONO_EXECUTABLE mono)
find_program(MCS_EXECUTABLE mcs)
find_program(GACUTIL_EXECUTABLE gacutil)
find_program(MSBUILD_EXECUTABLE msbuild)

set(MONO_FOUND FALSE CACHE INTERNAL "" FORCE)

set(MONO_ROOT_DIR "")
if(MONO_EXECUTABLE AND MCS_EXECUTABLE AND GACUTIL_EXECUTABLE AND MSBUILD_EXECUTABLE)
    set(MONO_FOUND_EXECUTABLES ON)
endif()

# macro, in win32: finds monoroot directory
macro(find_mono_root)
    set(MONO_ROOT_DIR "")
    set(PATHETIC_PATHS $ENV{PATH})
    LIST(APPEND PATHETIC_PATHS "C:/Program Files")
    LIST(APPEND PATHETIC_PATHS "C:/Program Files (x86)")
    set(FOUND OFF)
    foreach(pathe IN ITEMS ${PATHETIC_PATHS})
        #message("Searching for mono ${pathe}")
         if("${pathe}" MATCHES "Mono(\\|/|)$" AND EXISTS "${pathe}include")
            #message("Found mono ${pathe}")
            set(MONO_ROOT_DIR "${pathe}")
            break()
        endif()

        #check children
        file(GLOB SUBDIRSS "${pathe}/*")
        FOREACH(subdir ${SUBDIRSS})
            if(IS_DIRECTORY ${subdir})
               if("${subdir}" MATCHES "Mono(\\|/|)$" AND EXISTS "${subdir}/include")
                    #message("Found mono ${subdir}")
                    set(MONO_ROOT_DIR "${subdir}")
                    set(FOUND ON)
                    break()
                endif()
            endif() 
        ENDFOREACH()
        if(FOUND)
           break()
        endif()
    endforeach()
endmacro()

# mark mono as found if root folder exists
macro(check_root_existence)
if(EXISTS "${MONO_ROOT_DIR}")   
   set(MONO_FOUND TRUE CACHE INTERNAL "" FORCE)
else()
    message(WARNING "Cannot find mono root directory")
endif()
endmacro()

if(MONO_FOUND_EXECUTABLES OR MONO_ONLY_LIBRARIES_REQUIRED)
    if(APPLE)
        find_path(MONO_ROOT_DIR
                NAMES Mono/mono-2.0/mono/metadata/appdomain.h
                PATHS "/usr;/usr/local;")
        check_root_existence()

        set(MONO_INCLUDE_DIRS "${MONO_ROOT_DIR}/Headers/mono-2.0" CACHE PATH "Mono include directory" FORCE)
        set(MONO_LIBRARIES "${MONO_ROOT_DIR}/Libraries/libmonosgen-2.0.dylib" CACHE PATH "Mono library" FORCE)
        set(MONO_LIBRARY_DIRS "${MONO_ROOT_DIR}/Libraries" CACHE PATH "Mono library directory" FORCE)
    elseif(WIN32)
      find_mono_root()
      check_root_existence()
      message("root dir ${MONO_ROOT_DIR}")
      SET(MONO_INCLUDE_DIRS_BUFF
           ${MONO_ROOT_DIR}/include/mono-1.0
           ${MONO_ROOT_DIR}/include/mono-2.0
           ${MONO_ROOT_DIR}/include/glib-2.0
           ${MONO_ROOT_DIR}/lib/glib-2.0/include)    
      SET(MONO_INCLUDE_DIRS "")
      
      #extract only valid include dirs
      foreach(pathe IN ITEMS ${MONO_INCLUDE_DIRS_BUFF})
         if(EXISTS "${pathe}")
             LIST(APPEND MONO_INCLUDE_DIRS "${pathe}")
         endif()
      endforeach()
      if(NOT MONO_INCLUDE_DIRS)# no includes found
        set(MONO_FOUND FALSE CACHE INTERNAL "" FORCE)
      endif()
      set(MONO_INCLUDE_DIRS "${MONO_INCLUDE_DIRS}" CACHE PATH "Mono include directory" FORCE)
      set(MONO_LIBRARIES "mono-2.0-sgen.lib" CACHE PATH "Mono libraries" FORCE) 
      set(MONO_LIBRARY_DIRS "${MONO_ROOT_DIR}/lib" CACHE PATH "Mono library directory" FORCE)
        
    elseif(LINUX)
        find_path(MONO_ROOT_DIR
                NAMES include/mono-2.0/mono/metadata/appdomain.h
                PATHS "/usr;/usr/local;")
        
        check_root_existence()

        set(MONO_INCLUDE_DIRS "${MONO_ROOT_DIR}/include/mono-2.0" CACHE PATH "Mono include directories" FORCE)
        set(MONO_LIBRARIES "${MONO_ROOT_DIR}/lib/libmonosgen-2.0.a" CACHE PATH "Mono libraries" FORCE)
        set(MONO_LIBRARY_DIRS "${MONO_ROOT_DIR}/lib" CACHE PATH "Mono library directories" FORCE)
    endif()

    execute_process(COMMAND ${MCS_EXECUTABLE} --version OUTPUT_VARIABLE MONO_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE ".*version ([^ ]+)" "\\1" MONO_VERSION "${MONO_VERSION}")
endif()
mark_as_advanced(MONO_EXECUTABLE MCS_EXECUTABLE GACUTIL_EXECUTABLE MSBUILD_EXECUTABLE MONO_VERSION)