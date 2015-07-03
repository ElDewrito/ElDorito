cmake_minimum_required(VERSION 3.2)

add_library(MedalJunk STATIC
  Benchmark.cpp
  Benchmark.h
  codefinder.cpp
  Codefinder.h
  DebugHelper.cpp
  DebugHelper.h
  Direct3D.cpp
  Direct3D.h
  EnginePointers.cpp
  EnginePointers.h
  EngineTypes.h
  EventDispatcher.cpp
  EventDispatcher.h
  Events.h
  EventSubscriber.cpp
  EventSubscriber.h
  HookTest.cpp
  HookTest.h
  IHook.h
  InitHooks.cpp
  InitHooks.h
  libzplay.h
  main.cpp
  Optic.cpp
  Optic.h
  OpticAnimation.cpp
  OpticAnimation.h
  OpticAPI.cpp
  OpticAPI.h
  OpticBezier.cpp
  OpticBezier.h
  OpticEventHandler.cpp
  OpticEventHandler.h
  OpticException.h
  OpticHooks.cpp
  OpticHooks.h
  OpticLua.cpp
  OpticLua.h
  OpticMedal.cpp
  OpticMedal.h
  OpticMisc.cpp
  OpticMisc.h
  OpticPack.cpp
  OpticPack.h
  OpticPlayback.cpp
  OpticPlayback.h
  OpticRender.cpp
  OpticRender.h
  OpticSprite.cpp
  OpticSprite.h
  Patcher.cpp
  Patcher.h
  PatchGroup.cpp
  PatchGroup.h
  Shared.cpp
  Shared.h
  xde/xde.c
  xde/xde.h
  xzip/Clipboard.cpp
  xzip/Clipboard.h
  xzip/resource.h
  xzip/StdAfx.h
  xzip/XHyperLink.cpp
  xzip/XHyperLink.h
  xzip/XListBox.cpp
  xzip/XListBox.h
  xzip/xtrace.h
  xzip/XUnzip.cpp
  xzip/XUnzip.h
  xzip/XZip.cpp
  xzip/XZip.h
  xzip/XZipTest.cpp
  xzip/XZipTest.h
  xzip/XZipTestDlg.cpp
  xzip/XZipTestDlg.h)

create_source_groups(MedalJunk)

target_compile_definitions(MedalJunk PRIVATE MEDALJUNK_STATIC)

target_include_directories(MedalJunk PRIVATE
  ../lua-5.3.0/src
  $ENV{DXSDK_DIR}/Include)

target_link_libraries(MedalJunk zplay lua)