cmake_minimum_required(VERSION 3.2)

add_library(soundtouch STATIC
  source/SoundTouch/3dnow_win.cpp
  source/SoundTouch/AAFilter.cpp
  source/SoundTouch/AAFilter.h
  source/SoundTouch/BPMDetect.cpp
  source/SoundTouch/cpu_detect.h
  source/SoundTouch/cpu_detect_x86_win.cpp
  source/SoundTouch/FIFOSampleBuffer.cpp
  source/SoundTouch/FIRFilter.cpp
  source/SoundTouch/FIRFilter.h
  source/SoundTouch/mmx_optimized.cpp
  source/SoundTouch/PeakFinder.cpp
  source/SoundTouch/PeakFinder.h
  source/SoundTouch/RateTransposer.cpp
  source/SoundTouch/RateTransposer.h
  source/SoundTouch/SoundTouch.cpp
  source/SoundTouch/sse_optimized.cpp
  source/SoundTouch/TDStretch.cpp
  source/SoundTouch/TDStretch.h
  include/BPMDetect.h
  include/FIFOSampleBuffer.h
  include/FIFOSamplePipe.h
  include/SoundTouch.h
  include/STTypes.h)

create_source_groups(soundtouch)

target_include_directories(soundtouch PRIVATE include)