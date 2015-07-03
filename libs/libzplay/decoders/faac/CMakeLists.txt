cmake_minimum_required(VERSION 3.2)

add_library(faac STATIC
  libfaac/aacquant.c
  libfaac/aacquant.h
  libfaac/backpred.c
  libfaac/backpred.h
  libfaac/bitstream.h
  libfaac/bitstream1.c
  libfaac/channels.c
  libfaac/channels.h
  libfaac/coder.h
  libfaac/faac.h
  libfaac/faaccfg.h
  libfaac/fft.c
  libfaac/fft.h
  libfaac/filtbank.h
  libfaac/filtbank1.c
  libfaac/frame.h
  libfaac/frame1.c
  libfaac/huffman.h
  libfaac/huffman2.c
  libfaac/hufftab.h
  libfaac/kiss_fft/kiss_fft.c
  libfaac/kiss_fft/kiss_fft.h
  libfaac/kiss_fft/kiss_fftr.c
  libfaac/kiss_fft/kiss_fftr.h
  libfaac/kiss_fft/_kiss_fft_guts.h
  libfaac/ltp.c
  libfaac/ltp.h
  libfaac/midside.c
  libfaac/midside.h
  libfaac/psych.h
  libfaac/psychkni.c
  libfaac/tns.h
  libfaac/tns1.c
  libfaac/util.c
  libfaac/util.h
  libfaac/version.h)

create_source_groups(faac)