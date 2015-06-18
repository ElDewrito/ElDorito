cmake_minimum_required(VERSION 3.2)
project(ElDorito)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES
    Blam/BitStream.cpp
    Blam/BitStream.h
    Console/Console.cpp
    Console/Console.h
    Modules/ModuleBase.cpp
    Modules/ModuleBase.h
    Modules/ModuleCamera.cpp
    Modules/ModuleCamera.h
    Modules/ModuleGame.cpp
    Modules/ModuleGame.h
    Modules/ModuleInput.cpp
    Modules/ModuleInput.h
    Modules/ModulePlayer.cpp
    Modules/ModulePlayer.h
    Modules/ModuleServer.cpp
    Modules/ModuleServer.h
    Patches/Armor.cpp
    Patches/Armor.h
    Patches/ContentItems.cpp
    Patches/ContentItems.h
    Patches/Core.cpp
    Patches/Core.h
    Patches/Logging.cpp
    Patches/Logging.h
    Patches/Mouse.cpp
    Patches/Mouse.h
    Patches/Network.cpp
    Patches/Network.h
    Patches/PlayerPropertiesExtension.cpp
    Patches/PlayerPropertiesExtension.h
    Patches/PlayerUid.cpp
    Patches/PlayerUid.h
    Patches/RawInput.h
    Patches/RawInputPatch.cpp
    Patches/Scoreboard.cpp
    Patches/Scoreboard.h
    Patches/Ui.cpp
    Patches/Ui.h
    Patches/VirtualKeyboard.cpp
    Patches/VirtualKeyboard.h
    ThirdParty/rapidjson/error/en.h
    ThirdParty/rapidjson/error/error.h
    ThirdParty/rapidjson/internal/biginteger.h
    ThirdParty/rapidjson/internal/diyfp.h
    ThirdParty/rapidjson/internal/dtoa.h
    ThirdParty/rapidjson/internal/ieee754.h
    ThirdParty/rapidjson/internal/itoa.h
    ThirdParty/rapidjson/internal/meta.h
    ThirdParty/rapidjson/internal/pow10.h
    ThirdParty/rapidjson/internal/stack.h
    ThirdParty/rapidjson/internal/strfunc.h
    ThirdParty/rapidjson/internal/strtod.h
    ThirdParty/rapidjson/msinttypes/inttypes.h
    ThirdParty/rapidjson/msinttypes/stdint.h
    ThirdParty/rapidjson/allocators.h
    ThirdParty/rapidjson/document.h
    ThirdParty/rapidjson/encodedstream.h
    ThirdParty/rapidjson/encodings.h
    ThirdParty/rapidjson/filereadstream.h
    ThirdParty/rapidjson/filewritestream.h
    ThirdParty/rapidjson/memorybuffer.h
    ThirdParty/rapidjson/memorystream.h
    ThirdParty/rapidjson/pointer.h
    ThirdParty/rapidjson/prettywriter.h
    ThirdParty/rapidjson/rapidjson.h
    ThirdParty/rapidjson/reader.h
    ThirdParty/rapidjson/stringbuffer.h
    ThirdParty/rapidjson/writer.h
    ThirdParty/dirent.h
    ThirdParty/HttpRequest.cpp
    ThirdParty/HttpRequest.h
    ThirdParty/SHA256.cpp
    ThirdParty/SHA256.h
    Utils/Assert.cpp
    Utils/Assert.h
    Utils/Bits.h
    Utils/Macros.h
    Utils/Singleton.h
    Utils/String.cpp
    Utils/String.h
    Utils/Utils.h
    Utils/VersionInfo.cpp
    Utils/VersionInfo.h
    BlamTypes.h
    CommandMap.cpp
    CommandMap.h
    ElDorito.cpp
    ElDorito.h
    ElDoritoConfig.h
    ElMacros.h
    ElModules.cpp
    ElModules.h
    ElPatches.cpp
    ElPatches.h
    main.cpp
    Patch.cpp
    Patch.h
    Pointer.cpp
    Pointer.h
    Precompile.cpp
    Precompile.h
    resource.h
    ElDorito.rc
    ElDorito.def)

include_directories($ENV{DXSDK_DIR}/Include)
link_directories($ENV{DXSDK_DIR}/Lib/x86)

add_library(ElDorito MODULE ${SOURCE_FILES})

target_link_libraries(ElDorito d3d9)
target_link_libraries(ElDorito d3dx9)
target_link_libraries(ElDorito Version)
target_link_libraries(ElDorito ws2_32)

set_target_properties(ElDorito PROPERTIES OUTPUT_NAME "mtndew")