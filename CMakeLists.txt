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
    src/Blam/BitStream.h
    src/Console/ConsoleQueue.cpp
    src/Console/ConsoleQueue.h
    src/Console/DirectXHook.cpp
    src/Console/DirectXHook.h
    src/Console/GameChatQueue.cpp
    src/Console/GameChatQueue.h
    src/Console/GameConsole.cpp
    src/Console/GameConsole.h
    src/Console/GlobalChatQueue.cpp
    src/Console/GlobalChatQueue.h
    src/Console/IRCBackend.cpp
    src/Console/IRCBackend.h
    src/Console/KeyboardHook.cpp
    src/Console/KeyboardHook.h
    src/Console/Queue.cpp
    src/Console/Queue.h
    src/Modules/ModuleBase.cpp
    src/Modules/ModuleBase.h
    src/Modules/ModuleCamera.cpp
    src/Modules/ModuleCamera.h
    src/Modules/ModuleGame.cpp
    src/Modules/ModuleGame.h
    src/Modules/ModuleInput.cpp
    src/Modules/ModuleInput.h
    src/Modules/ModulePlayer.cpp
    src/Modules/ModulePlayer.h
    src/Modules/ModuleServer.cpp
    src/Modules/ModuleServer.h
    src/Patches/Armor.cpp
    src/Patches/Armor.h
    src/Patches/ContentItems.cpp
    src/Patches/ContentItems.h
    src/Patches/Core.cpp
    src/Patches/Core.h
    src/Patches/Logging.cpp
    src/Patches/Logging.h
    src/Patches/Mouse.cpp
    src/Patches/Mouse.h
    src/Patches/Network.cpp
    src/Patches/Network.h
    src/Patches/PlayerPropertiesExtension.cpp
    src/Patches/PlayerPropertiesExtension.h
    src/Patches/PlayerUid.cpp
    src/Patches/PlayerUid.h
    src/Patches/RawInput.h
    src/Patches/RawInputPatch.cpp
    src/Patches/Scoreboard.cpp
    src/Patches/Scoreboard.h
    src/Patches/Ui.cpp
    src/Patches/Ui.h
    src/Patches/VirtualKeyboard.cpp
    src/Patches/VirtualKeyboard.h
    src/ThirdParty/rapidjson/error/en.h
    src/ThirdParty/rapidjson/error/error.h
    src/ThirdParty/rapidjson/internal/biginteger.h
    src/ThirdParty/rapidjson/internal/diyfp.h
    src/ThirdParty/rapidjson/internal/dtoa.h
    src/ThirdParty/rapidjson/internal/ieee754.h
    src/ThirdParty/rapidjson/internal/itoa.h
    src/ThirdParty/rapidjson/internal/meta.h
    src/ThirdParty/rapidjson/internal/pow10.h
    src/ThirdParty/rapidjson/internal/stack.h
    src/ThirdParty/rapidjson/internal/strfunc.h
    src/ThirdParty/rapidjson/internal/strtod.h
    src/ThirdParty/rapidjson/msinttypes/inttypes.h
    src/ThirdParty/rapidjson/msinttypes/stdint.h
    src/ThirdParty/rapidjson/allocators.h
    src/ThirdParty/rapidjson/document.h
    src/ThirdParty/rapidjson/encodedstream.h
    src/ThirdParty/rapidjson/encodings.h
    src/ThirdParty/rapidjson/filereadstream.h
    src/ThirdParty/rapidjson/filewritestream.h
    src/ThirdParty/rapidjson/memorybuffer.h
    src/ThirdParty/rapidjson/memorystream.h
    src/ThirdParty/rapidjson/pointer.h
    src/ThirdParty/rapidjson/prettywriter.h
    src/ThirdParty/rapidjson/rapidjson.h
    src/ThirdParty/rapidjson/reader.h
    src/ThirdParty/rapidjson/stringbuffer.h
    src/ThirdParty/rapidjson/writer.h
    src/ThirdParty/dirent.h
    src/ThirdParty/HttpRequest.cpp
    src/ThirdParty/HttpRequest.h
    src/ThirdParty/SHA256.cpp
    src/ThirdParty/SHA256.h
    src/Utils/Assert.cpp
    src/Utils/Assert.h
    src/Utils/Bits.h
    src/Utils/Cryptography.cpp
    src/Utils/Cryptography.h
    src/Utils/Macros.h
    src/Utils/Singleton.h
    src/Utils/String.cpp
    src/Utils/String.h
    src/Utils/Utils.h
    src/Utils/VersionInfo.cpp
    src/Utils/VersionInfo.h
    src/BlamTypes.h
    src/CommandMap.cpp
    src/CommandMap.h
    src/ElDorito.cpp
    src/ElDorito.h
    src/ElDoritoConfig.h
    src/ElModules.cpp
    src/ElModules.h
    src/ElPatches.cpp
    src/ElPatches.h
    src/main.cpp
    src/Patch.cpp
    src/Patch.h
    src/Pointer.cpp
    src/Pointer.h
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