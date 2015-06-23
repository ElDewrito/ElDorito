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

# Add all source files
set(SOURCE_FILES
    src/Blam/BitStream.cpp
    src/Blam/BitStream.hpp
    src/Blam/BlamTypes.hpp
    src/Console/ConsoleQueue.cpp
    src/Console/ConsoleQueue.hpp
    src/Console/DirectXHook.cpp
    src/Console/DirectXHook.hpp
    src/Console/GameChatQueue.cpp
    src/Console/GameChatQueue.hpp
    src/Console/GameConsole.cpp
    src/Console/GameConsole.hpp
    src/Console/GlobalChatQueue.cpp
    src/Console/GlobalChatQueue.hpp
    src/Console/IRCBackend.cpp
    src/Console/IRCBackend.hpp
    src/Console/KeyboardHook.cpp
    src/Console/KeyboardHook.hpp
    src/Console/Queue.cpp
    src/Console/Queue.hpp
    src/Modules/ModuleBase.cpp
    src/Modules/ModuleBase.hpp
    src/Modules/ModuleCamera.cpp
    src/Modules/ModuleCamera.hpp
    src/Modules/ModuleGame.cpp
    src/Modules/ModuleGame.hpp
    src/Modules/ModuleInput.cpp
    src/Modules/ModuleInput.hpp
    src/Modules/ModulePlayer.cpp
    src/Modules/ModulePlayer.hpp
    src/Modules/ModuleServer.cpp
    src/Modules/ModuleServer.hpp
    src/Patches/Armor.cpp
    src/Patches/Armor.hpp
    src/Patches/ContentItems.cpp
    src/Patches/ContentItems.hpp
    src/Patches/Core.cpp
    src/Patches/Core.hpp
    src/Patches/Logging.cpp
    src/Patches/Logging.hpp
    src/Patches/Mouse.cpp
    src/Patches/Mouse.hpp
    src/Patches/Network.cpp
    src/Patches/Network.hpp
    src/Patches/PlayerPropertiesExtension.cpp
    src/Patches/PlayerPropertiesExtension.hpp
    src/Patches/PlayerUid.cpp
    src/Patches/PlayerUid.hpp
    src/Patches/RawInput.hpp
    src/Patches/RawInputPatch.cpp
    src/Patches/Scoreboard.cpp
    src/Patches/Scoreboard.hpp
    src/Patches/Ui.cpp
    src/Patches/Ui.hpp
    src/Patches/VirtualKeyboard.cpp
    src/Patches/VirtualKeyboard.hpp
    src/ThirdParty/dirent.h
    src/ThirdParty/HttpRequest.cpp
    src/ThirdParty/HttpRequest.hpp
    src/Utils/Assert.cpp
    src/Utils/Assert.hpp
    src/Utils/Bits.hpp
    src/Utils/Cryptography.cpp
    src/Utils/Cryptography.hpp
    src/Utils/Macros.hpp
    src/Utils/Singleton.hpp
    src/Utils/String.cpp
    src/Utils/String.hpp
    src/Utils/Utils.hpp
    src/Utils/VersionInfo.cpp
    src/Utils/VersionInfo.hpp
    src/CommandMap.cpp
    src/CommandMap.hpp
    src/ElDorito.cpp
    src/ElDorito.hpp
    src/ElModules.cpp
    src/ElModules.hpp
    src/ElPatches.cpp
    src/ElPatches.hpp
    src/main.cpp
    src/Patch.cpp
    src/Patch.hpp
    src/Pointer.cpp
    src/Pointer.hpp
    src/resource.h
    src/ElDorito.rc
    src/ElDorito.def)

# Include directories
include_directories($ENV{DXSDK_DIR}/Include)
include_directories(${CMAKE_SOURCE_DIR}/libs/detours/include)
include_directories(${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/include)

# Library directories
link_directories($ENV{DXSDK_DIR}/Lib/x86)
link_directories(${CMAKE_SOURCE_DIR}/libs/detours/lib.X86)
link_directories(${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/lib)

# Generate DLL from source files
add_library(ElDorito SHARED ${SOURCE_FILES})

# Preserve directory structure
foreach(FILE ${SOURCE_FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  # skip src or include and changes /'s to \\'s
  string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  # group into "Source Files" and "Header Files"
  if ("${FILE}" MATCHES ".*\\.cpp")
    set(GROUP "Source Files\\${GROUP}")
  elseif("${FILE}" MATCHES ".*\\.h")
    set(GROUP "Header Files\\${GROUP}")
  endif()

  source_group("${GROUP}" FILES "${FILE}")
endforeach()

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