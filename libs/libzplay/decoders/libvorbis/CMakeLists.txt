cmake_minimum_required(VERSION 3.2)

add_library(vorbis STATIC
  lib/analysis.c
  lib/bitrate.c
  lib/block.c
  lib/codebook.c
  lib/envelope.c
  lib/floor0.c
  lib/floor1.c
  lib/info.c
  lib/lookup.c
  lib/lpc.c
  lib/lsp.c
  lib/mapping0.c
  lib/mdct.c
  lib/psy.c
  lib/registry.c
  lib/res0.c
  lib/sharedbook.c
  lib/smallft.c
  lib/synthesis.c
  lib/window.c
  lib/bitrate.h
  include/vorbis/codec.h
  lib/codec_internal.h
  lib/envelope.h
  lib/highlevel.h
  lib/lookup.h
  lib/lookup_data.h
  lib/lpc.h
  lib/lsp.h
  lib/masking.h
  lib/mdct.h
  lib/misc.h
  lib/os.h
  lib/psy.h
  lib/registry.h
  lib/scales.h
  lib/smallft.h
  lib/window.h)

create_source_groups(vorbis)

target_include_directories(vorbis PRIVATE include)