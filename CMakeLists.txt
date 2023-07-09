#########################################
#            CHIP 8 Emulator            #
#---------------------------------------#
# @author Nikolaus Rauch                #
# @date 27.12.2021                      #
#---------------------------------------#
#########################################
cmake_minimum_required( VERSION 3.9 )
project( chip-8-emu VERSION 1.0.0 LANGUAGES CXX )

message("\n * Chip-8 Emulator ")
message("\n * https://github.com/nikolausrauch/chip-8-emu \n")

message(STATUS "${PROJECT_NAME} build environment --")
message(STATUS "Build realm:     ${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")

#########################################
#                Options                #
#########################################
option(BUILD_SFML  "Build SFML from source" ON)


#########################################
#              CMake-Stuff              #
#########################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


#########################################
#        Global Compiler Options        #
#########################################
set(GCC_COMPILE_OPTIONS "-Wall")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-ggdb;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(MSVC_COMPILE_OPTIONS "/Wall")
set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS};/Zi")
set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS};/O2")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GCC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GCC_COMPILE_RELEASE_OPTIONS}>")

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:${MSVC_COMPILE_RELEASE_OPTIONS}>")



#################################
#   Build external libraries    #
#################################
set( OpenGL_GL_PREFERENCE GLVND )
find_package(OpenGL REQUIRED)

if(BUILD_SFML)
    add_subdirectory(external/sfml)

    if(WIN32)
        file(COPY external/sfml/extlibs/bin/x64/openal32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

else()
    find_package( SFML 2.5 COMPONENTS system window graphics REQUIRED )
endif()

#################################
#         Emulator Source       #
#################################
set( EMU_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/chip8/chip8.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/chip8/instruction.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/viewer.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/chip8_emu.cpp"
    )

set( EMU_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/chip8/base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/chip8/chip8.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/chip8/instruction.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/window.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/viewer/viewer.h"
    )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${EMU_SRC} ${EMU_HDR} )


#################################
#         Build Emulator        #
#################################
add_executable( chip-8-emu ${EMU_SRC} ${EMU_HDR} )
target_link_libraries( chip-8-emu PRIVATE sfml-system sfml-window sfml-graphics )
target_include_directories( chip-8-emu PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    )

target_compile_features( chip-8-emu PUBLIC cxx_std_20 )
set_target_properties( chip-8-emu PROPERTIES CXX_EXTENSIONS OFF )

