cmake_minimum_required(VERSION 3.2)

add_library(mad STATIC
  bit.c
  bit.h
  decoder.c
  decoder.h
  fixed.c
  fixed.h
  frame.c
  frame.h
  global.h
  huffman.c
  huffman.h
  layer12.c
  layer12.h
  layer3.c
  layer3.h
  mad.h
  config.h
  stream.c
  stream.h
  synth.c
  synth.h
  timer.c
  timer.h
  version.c
  version.h)

create_source_groups(mad)

target_compile_definitions(mad PRIVATE
  FPM_INTEL
  HAVE_CONFIG_H
  ASO_ZEROCHECK)