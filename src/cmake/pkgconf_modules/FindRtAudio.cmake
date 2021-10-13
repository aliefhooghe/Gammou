
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkgconf_modules/PkgConfigHelper.cmake)

find_pkgconfig_module(
  PACKAGE_NAME RtAudio
  PKGCONF_MODULE rtaudio
  TARGET RtAudio::rtaudio
  INCLUDE_DIR_HINT RtAudio.h
  LIBRARY_HINT rtaudio
)
