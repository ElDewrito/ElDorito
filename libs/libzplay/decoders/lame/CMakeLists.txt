cmake_minimum_required(VERSION 3.2)

add_library(lame STATIC
  config.h
  include/lame.h
  libmp3lame/bitstream.c
  libmp3lame/bitstream.h
  libmp3lame/encoder.c
  libmp3lame/encoder.h
  libmp3lame/fft.c
  libmp3lame/fft.h
  libmp3lame/gain_analysis.c
  libmp3lame/gain_analysis.h
  libmp3lame/i386/nasm.h
  libmp3lame/id3tag.c
  libmp3lame/id3tag.h
  libmp3lame/l3side.h
  libmp3lame/lame-analysis.h
  libmp3lame/lame.c
  libmp3lame/lame_global_flags.h
  libmp3lame/machine.h
  libmp3lame/mpglib_interface.c
  libmp3lame/newmdct.c
  libmp3lame/newmdct.h
  libmp3lame/presets.c
  libmp3lame/psymodel.c
  libmp3lame/psymodel.h
  libmp3lame/quantize.c
  libmp3lame/quantize.h
  libmp3lame/quantize_pvt.c
  libmp3lame/quantize_pvt.h
  libmp3lame/reservoir.c
  libmp3lame/reservoir.h
  libmp3lame/set_get.c
  libmp3lame/set_get.h
  libmp3lame/tables.c
  libmp3lame/tables.h
  libmp3lame/takehiro.c
  libmp3lame/util.c
  libmp3lame/util.h
  libmp3lame/vbrquantize.c
  libmp3lame/vbrquantize.h
  libmp3lame/VbrTag.c
  libmp3lame/VbrTag.h
  libmp3lame/vector/lame_intrin.h
  libmp3lame/vector/xmm_quantize_sub.c
  libmp3lame/version.c
  libmp3lame/version.h)

create_source_groups(lame)

target_compile_definitions(lame PRIVATE HAVE_CONFIG_H)

target_include_directories(lame PRIVATE
  .
  libmp3lame
  include)