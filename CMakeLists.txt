cmake_minimum_required(VERSION 3.2)

# Statically link MSVC++ runtime to output
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)

# Set HALO_ROOT
set(HALO_ROOT "C:/Halo/" CACHE PATH "Root of the Halo install")

# Project name is ElDorito
project(ElDorito)

# Add CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Find all source files
file(GLOB_RECURSE SOURCE_FILES
    "src/*.h"
    "src/*.cpp"
    "src/ElDorito.rc"
    "src/ElDorito.def"
)

# Include directories
include_directories($ENV{DXSDK_DIR}/Include)
include_directories(${CMAKE_SOURCE_DIR}/libs/detours/include)
include_directories(${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/include)

# Library directories
link_directories($ENV{DXSDK_DIR}/Lib/x86)
link_directories(${CMAKE_SOURCE_DIR}/libs/detours/lib.X86)
link_directories(${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/lib)

# Generate DLL from source files
add_library(ElDorito MODULE ${SOURCE_FILES})

# Copy DLL to HALO_ROOT
if(IS_DIRECTORY ${HALO_ROOT})
  add_custom_command(TARGET ElDorito POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy $<TARGET_FILE:ElDorito>
                              ${HALO_ROOT}/$<TARGET_FILE_NAME:ElDorito>)
endif(IS_DIRECTORY ${HALO_ROOT})

# Link lib files
target_link_libraries(ElDorito d3d9)
target_link_libraries(ElDorito d3dx9)
target_link_libraries(ElDorito Version)
target_link_libraries(ElDorito ws2_32)
target_link_libraries(ElDorito detours)
target_link_libraries(ElDorito libeay32MT)

# Rename output to mtndew
set_target_properties(ElDorito PROPERTIES OUTPUT_NAME "mtndew")