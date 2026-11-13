cmake_minimum_required(VERSION 3.22)
project(game C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(SDL)

include(FetchContent)

# openal
set(LIBTYPE STATIC CACHE STRING "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(openal
  GIT_REPOSITORY https://github.com/kcat/openal-soft.git
  GIT_TAG 1.24.3)
FetchContent_MakeAvailable(openal)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_SKIP_INSTALL_RULES ON)

# zlib
find_package(ZLIB REQUIRED)

# libpng
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_STATIC ON CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "" FORCE)
set(PNG_EXECUTABLES OFF CACHE BOOL "" FORCE)
set(PNG_FRAMEWORK OFF CACHE BOOL "" FORCE)
FetchContent_Declare(libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG v1.6.47)
FetchContent_MakeAvailable(libpng)

# libjpeg-turbo
include(ExternalProject)
set(JPEG_PREFIX ${CMAKE_BINARY_DIR}/jpeg-install)

ExternalProject_Add(libjpeg_turbo_ext
  GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
  GIT_TAG 3.0.4
  GIT_SHALLOW ON
  PREFIX ${CMAKE_BINARY_DIR}/jpeg-build
  CMAKE_ARGS
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DANDROID_ABI=${ANDROID_ABI}
    -DANDROID_PLATFORM=${ANDROID_PLATFORM}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
    -DCMAKE_INSTALL_PREFIX=${JPEG_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    -DENABLE_SHARED=OFF
    -DENABLE_STATIC=ON
    -DWITH_TURBOJPEG=ON
    -DWITH_JAVA=OFF
  BUILD_BYPRODUCTS ${JPEG_PREFIX}/lib/libturbojpeg.a
)

file(MAKE_DIRECTORY ${JPEG_PREFIX}/include)

add_library(turbojpeg_lib STATIC IMPORTED GLOBAL)
set_target_properties(turbojpeg_lib PROPERTIES
  IMPORTED_LOCATION ${JPEG_PREFIX}/lib/libturbojpeg.a
  INTERFACE_INCLUDE_DIRECTORIES ${JPEG_PREFIX}/include)
add_dependencies(turbojpeg_lib libjpeg_turbo_ext)

# libogg
set(INSTALL_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(ogg
  GIT_REPOSITORY https://github.com/xiph/ogg.git
  GIT_TAG v1.3.5)
FetchContent_MakeAvailable(ogg)

if(NOT TARGET Ogg::Ogg)
  add_library(Ogg::Ogg ALIAS ogg)
endif()
set(OGG_INCLUDE_DIR "${ogg_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(OGG_LIBRARY ogg CACHE STRING "" FORCE)

# libvorbis
FetchContent_Declare(vorbis
  GIT_REPOSITORY https://github.com/xiph/vorbis.git
  GIT_TAG v1.3.7)
FetchContent_MakeAvailable(vorbis)

# mikktspace
FetchContent_Declare(mikktspace
  GIT_REPOSITORY https://github.com/mmikk/MikkTSpace.git
  GIT_TAG master)
FetchContent_MakeAvailable(mikktspace)

# mikktspace.h
file(COPY ${mikktspace_SOURCE_DIR}/mikktspace.h
     DESTINATION ${CMAKE_BINARY_DIR}/mikkt_inc/mikktspace)
add_library(mikktspace STATIC ${mikktspace_SOURCE_DIR}/mikktspace.c)
target_include_directories(mikktspace PUBLIC
  ${mikktspace_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/mikkt_inc)

# HashLink 1.16
FetchContent_Declare(hashlink
  URL https://github.com/gts1108/hashlink/archive/refs/tags/1.16.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_SUBDIR _no_cmake_)
FetchContent_MakeAvailable(hashlink)
set(HL "${hashlink_SOURCE_DIR}")

file(GLOB HL_STD  ${HL}/src/std/*.c)
file(GLOB HL_FMT  ${HL}/libs/fmt/*.c)
file(GLOB HL_PCRE ${HL}/include/pcre/*.c)

list(FILTER HL_PCRE EXCLUDE REGEX
  "pcre2_jit_match\\.c$|pcre2_jit_misc\\.c$|pcre2_jit_test\\.c$|pcre2_printint\\.c$|pcre2_ucptables\\.c$|pcre2_dftables\\.c$|pcre2_fuzzsupport\\.c$|pcre2grep\\.c$|pcre2test\\.c$|pcre2_chartables\\.c\\.dist$")

add_library(main SHARED
  src/main.c
  src/hlc/game.c
  ${HL_PCRE}
  ${HL}/src/gc.c ${HL_STD} ${HL_FMT}
  ${HL}/libs/sdl/sdl.c ${HL}/libs/sdl/gl.c
  ${HL}/libs/openal/openal.c
  ${HL}/libs/ui/ui_stub.c
  src/openal_stubs.c
)
target_include_directories(main PRIVATE
  src/hlc ${HL}/src ${HL}/include
  ${HL}/include/pcre
  ${HL}/include/minimp3
  ${libpng_SOURCE_DIR} ${libpng_BINARY_DIR}
)
target_compile_definitions(main PRIVATE
  PCRE2_CODE_UNIT_WIDTH=16
  PCRE2_STATIC
  HAVE_CONFIG_H
  OPENAL_STATIC)
  
target_link_libraries(main PRIVATE
  SDL3::SDL3 OpenAL
  png_static turbojpeg_lib
  vorbisfile vorbis ogg
  mikktspace ZLIB::ZLIB
  log android GLESv3 EGL m)

set_source_files_properties(src/hlc/game.c PROPERTIES COMPILE_DEFINITIONS "main=hl_game_main")