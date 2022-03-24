
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkgconf_modules/PkgConfigHelper.cmake)

find_pkgconfig_module(
  PACKAGE_NAME RtMidi
  PKGCONF_MODULE rtmidi
  TARGET RtMidi::rtmidi
  INCLUDE_DIR_HINT RtMidi.h
  LIBRARY_HINT rtmidi
)
