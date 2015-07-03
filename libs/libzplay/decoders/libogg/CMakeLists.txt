cmake_minimum_required(VERSION 3.2)

add_library(ogg STATIC
  include/ogg/ogg.h
  include/ogg/os_types.h
  src/bitwise.c
  src/framing.c)

create_source_groups(ogg)

target_include_directories(ogg PRIVATE include)