cmake_minimum_required(VERSION 3.2)

# Creates source groups for a target
function(create_source_groups TARGET)
  get_property(SOURCES TARGET ${TARGET} PROPERTY SOURCES)
  foreach(FILE ${SOURCES}) 
    get_filename_component(PARENT_DIR "${FILE}" PATH)

    # skip src or include and changes /'s to \\'s
    string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES "\\.c(pp)?$")
      set(GROUP "Source Files\\${GROUP}")
    elseif("${FILE}" MATCHES "\\.h(pp)?$")
      set(GROUP "Header Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
  endforeach()

  # Create a solution folder for the target based on the directory this
  # function was called from

  # There doesn't seem to be a function to convert an absolute path to a
  # relative one, so hack around it by getting the length of the root directory
  # and chopping it off of the beginning of the target's directory
  string(LENGTH "${CMAKE_SOURCE_DIR}" SOURCE_DIR_LENGTH)
  string(SUBSTRING "${CMAKE_CURRENT_SOURCE_DIR}" ${SOURCE_DIR_LENGTH} -1 FOLDER_NAME)
  set_target_properties(${TARGET} PROPERTIES FOLDER "${FOLDER_NAME}")
endfunction()

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
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Subdirectories
add_subdirectory(libs/lua-5.3.0)
add_subdirectory(libs/libzplay)
add_subdirectory(libs/MedalJunk)

# Library directories
link_directories($ENV{DXSDK_DIR}/Lib/x86)
link_directories(${CMAKE_SOURCE_DIR}/libs/detours/lib.X86)
link_directories(${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/lib)
link_directories(${CMAKE_SOURCE_DIR}/libs/teamspeak/lib)
link_directories(${CMAKE_SOURCE_DIR}/libs/libwebsockets/lib)

# Create main target
add_library(ElDorito SHARED
    src/Blam/BitStream.cpp
    src/Blam/BitStream.hpp
    src/Blam/BlamInput.cpp
    src/Blam/BlamInput.hpp
    src/Blam/BlamNetwork.cpp
    src/Blam/BlamNetwork.hpp
    src/Blam/BlamTypes.hpp
    src/Blam/Tags/Tags.hpp
    src/Blam/Tags/GameEngineSettingsDefinition.hpp
    src/Blam/Tags/Scenario.hpp
    src/DirectXHook.cpp
    src/DirectXHook.hpp
    src/Console/GameConsole.cpp
    src/Console/GameConsole.hpp
    src/Console/IRCBackend.cpp
    src/Console/IRCBackend.hpp
    src/Console/Queue.hpp
    src/Console/Queue.cpp
    src/Modules/ModuleBase.cpp
    src/Modules/ModuleBase.hpp
    src/Modules/ModuleCamera.cpp
    src/Modules/ModuleCamera.hpp
    src/Modules/ModuleGame.cpp
    src/Modules/ModuleGame.hpp
    src/Modules/ModuleInput.cpp
    src/Modules/ModuleInput.hpp
    src/Modules/ModuleIRC.cpp
    src/Modules/ModuleIRC.hpp
    src/Modules/ModulePlayer.cpp
    src/Modules/ModulePlayer.hpp
    src/Modules/ModuleServer.cpp
    src/Modules/ModuleServer.hpp    
    src/Modules/ModuleVoIP.cpp
    src/Modules/ModuleVoIP.hpp
    src/Modules/ModuleTime.cpp
    src/Modules/ModuleTime.hpp
    src/Modules/ModuleGraphics.cpp
    src/Modules/ModuleGraphics.hpp
    src/Patches/Armor.cpp
    src/Patches/Armor.hpp
    src/Patches/ContentItems.cpp
    src/Patches/ContentItems.hpp
    src/Patches/CustomPackets.cpp
    src/Patches/CustomPackets.hpp
    src/Patches/Core.cpp
    src/Patches/Core.hpp
    src/Patches/Forge.cpp
    src/Patches/Forge.hpp
    src/Patches/Input.cpp
    src/Patches/Input.hpp
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
    src/Patches/Sprint.cpp
    src/Patches/Sprint.hpp
    src/Patches/Ui.cpp
    src/Patches/Ui.hpp
    src/Patches/VirtualKeyboard.cpp
    src/Patches/VirtualKeyboard.hpp
    src/Server/BanList.cpp
    src/Server/BanList.hpp
    src/Server/ServerChat.cpp
    src/Server/ServerChat.hpp
    src/Server/VariableSynchronization.cpp
    src/Server/VariableSynchronization.hpp
    src/ThirdParty/dirent.h
    src/ThirdParty/HttpRequest.cpp
    src/ThirdParty/HttpRequest.hpp
    src/ThirdParty/WebSockets.cpp
    src/ThirdParty/WebSockets.hpp
    src/Utils/AntiSpeedHack.cpp
    src/Utils/AntiSpeedHack.hpp
    src/Utils/Assert.cpp
    src/Utils/Assert.hpp
    src/Utils/Bits.hpp
    src/Utils/Cryptography.cpp
    src/Utils/Cryptography.hpp
    src/Utils/DebugLog.cpp
    src/Utils/DebugLog.hpp
    src/Utils/Macros.hpp
    src/Utils/Singleton.hpp
    src/Utils/String.cpp
    src/Utils/String.hpp
    src/Utils/Utils.hpp
    src/Utils/VersionInfo.cpp
    src/Utils/VersionInfo.hpp
    src/VoIP/MemberList.cpp
    src/VoIP/MemberList.hpp
    src/VoIP/TeamspeakClient.cpp
    src/VoIP/TeamspeakClient.hpp
    src/VoIP/TeamspeakServer.cpp
    src/VoIP/TeamspeakServer.hpp
    src/Menu.cpp
    src/Menu.hpp
    src/Settings.hpp
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
    src/ForgeSavingDialog.rc
    src/ElDorito.def)

create_source_groups(ElDorito)

# Include directories
target_include_directories(ElDorito PRIVATE
  $ENV{DXSDK_DIR}/Include
  ${CMAKE_SOURCE_DIR}/libs/detours/include
  ${CMAKE_SOURCE_DIR}/libs/openssl-1.0.2c/include
  ${CMAKE_SOURCE_DIR}/libs/teamspeak/include
  ${CMAKE_SOURCE_DIR}/libs/libwebsockets/include)

# Copy DLL to HALO_ROOT
if(IS_DIRECTORY ${HALO_ROOT})
  add_custom_command(TARGET ElDorito POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy $<TARGET_FILE:ElDorito>
                              ${HALO_ROOT}/$<TARGET_FILE_NAME:ElDorito>)
  add_custom_command(TARGET ElDorito POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${CMAKE_CURRENT_SOURCE_DIR}/artifact_extras/custom_menu.exe
                              ${HALO_ROOT}/custom_menu.exe)
endif(IS_DIRECTORY ${HALO_ROOT})

# Link lib files
target_link_libraries(ElDorito
  d3d9
  d3dx9
  Version
  ws2_32
  detours
  libeay32MT
  ts3client_win32
  ts3client_win64
  ts3server_win32
  ts3server_win64
  ZLib
  libwebsocketswin32
  MedalJunk
  psapi)

target_compile_definitions(ElDorito PRIVATE NOMINMAX) # Prevent windows.h from defining min() and max()
target_compile_definitions(ElDorito PRIVATE PSAPI_VERSION=1)

# Rename output to mtndew
set_target_properties(ElDorito PROPERTIES OUTPUT_NAME "mtndew")