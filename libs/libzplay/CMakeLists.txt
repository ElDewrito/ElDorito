cmake_minimum_required(VERSION 3.2)

add_subdirectory(decoders/faac)
add_subdirectory(decoders/faad)
add_subdirectory(decoders/lame)
add_subdirectory(decoders/libmad)
add_subdirectory(decoders/libogg)
add_subdirectory(decoders/libvorbis)
add_subdirectory(decoders/soundtouch)

add_library(zplay STATIC
  src/config.h
  src/debug.h
  src/libzplay.cpp
  src/waudioprocessor.cpp
  src/waudioprocessor.h
  src/wcentercutprocessor.cpp
  src/wcentercutprocessor.h
  src/wdecoder.cpp
  src/wdecoder.h
  src/wechoprocessor.cpp
  src/wechoprocessor.h
  src/weqprocessor.cpp
  src/weqprocessor.h
  src/wfft.cpp
  src/wfft.h
  src/winputprocessor.cpp
  src/winputprocessor.h
  src/wmp3decoder.cpp
  src/wmp3decoder.h
  src/wmp3x.cpp
  src/wmp3x.h
  src/woutputprocessor.cpp
  src/woutputprocessor.h
  src/wqueue.cpp
  src/wqueue.h
  src/wstprocessor.cpp
  src/wstprocessor.h
  src/wtags.cpp
  src/wtags.h
  src/wtools.cpp
  src/wtools.h
  src/wwavein.cpp
  src/wwavein.h
  src/wwavelet.cpp
  src/wwavelet.h
  include/libzplay.h)

create_source_groups(zplay)

target_compile_definitions(zplay PRIVATE
  LIB_ZPLAY_STATIC
  _CRT_SECURE_NO_WARNINGS
  FLAC__NO_DLL)

target_include_directories(zplay PRIVATE include)

target_link_libraries(zplay
  faac
  faad
  lame
  mad
  ogg
  vorbis
  soundtouch)