cmake_minimum_required(VERSION 2.8)
project(SDL_PNGLITE C)

set(HAVE_SDLIMAGE2 OFF)

option(BUILD_SHARED_LIBS "Build Shared Library" OFF)
option(TRACE_EXECUTION "Trace execution" OFF)
option(TRACE_EXECUTION_DEINTERLACE "Trace deinterlace execution" OFF)
option(TRACE_EXECUTION_DESTRUCTIVE "Trace deinterlace execution destructively" OFF)

set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif(BUILD_SHARED_LIBS)

if (TRACE_EXECUTION)
  add_definitions(-DTRACE)
endif()
if (TRACE_EXECUTION_DEINTERLACE)
  add_definitions(-DTRACE_DEINTERLACE)
endif()
if (TRACE_EXECUTION_DESTRUCTIVE)
  add_definitions(-DTRACE_DESTRUCTIVE)
endif()

if(NOT (WINDOWS OR CYGWIN))
  set(prefix ${CMAKE_INSTALL_PREFIX})
  set(exec_prefix "\${prefix}")
  set(libdir "\${exec_prefix}/lib${LIB_SUFFIX}")
  set(bindir "\${exec_prefix}/bin")
  set(includedir "\${prefix}/include")
endif()

if (CMAKE_HOST_UNIX)
  set(ENV{PKG_CONFIG_PATH} "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig/")

  set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -fvisibility=hidden")
  set(CMAKE_C_FLAGS_DEBUG "-ggdb3")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -ggdb3")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -ggdb3")

  include(FindPkgConfig)
  pkg_check_modules(PKG_SDL2 REQUIRED sdl2)
  pkg_check_modules(PKG_ZLIB REQUIRED zlib)
  pkg_check_modules(PKG_SDL2IMAGE SDL2_image)
  if (PKG_SDL2IMAGE_FOUND)
    set(HAVE_SDLIMAGE2 ON)
  endif(PKG_SDL2IMAGE_FOUND)
endif(CMAKE_HOST_UNIX)

if (MSVC)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)

  set(PKG_SDL2_INCLUDE_DIRS "" CACHE PATH "Location of SDL2 include files")
  set(PKG_SDL2_LIBRARY_DIRS "" CACHE PATH "Location of SDL2 library files")
  set(PKG_SDL2IMAGE_INCLUDE_DIRS "" CACHE PATH "Location of SDL2_image include files")
  set(PKG_SDL2IMAGE_LIBRARY_DIRS "" CACHE PATH "Location of SDL2_image library files")
  set(PKG_ZLIB_INCLUDE_DIRS "" CACHE PATH "Location of zlib include files")
  set(PKG_ZLIB_LIBRARY_DIRS "" CACHE PATH "Location of zlib library files")

  set(PKG_SDL2_LIBRARIES SDL2 SDL2main)
  set(PKG_SDL2IMAGE_LIBRARIES SDL2_image)
  set(PKG_ZLIB_LIBRARIES zdll)
endif(MSVC)

configure_file("${SDL_PNGLITE_SOURCE_DIR}/sdl_pnglite.pc.in" "${SDL_PNGLITE_BINARY_DIR}/sdl_pnglite.pc" @ONLY)

include_directories(${PKG_SDL2_INCLUDE_DIRS} ${PKG_SDL2IMAGE_INCLUDE_DIRS} ${PKG_ZLIB_INCLUDE_DIRS} )
link_directories(${PKG_SDL2_LIBRARY_DIRS} ${PKG_SDL2IMAGE_LIBRARY_DIRS} ${PKG_ZLIB_LIBRARY_DIRS} )

add_library(SDL_pnglite ${LIB_TYPE} SDL_pnglite.c pnglite.c)
target_link_libraries(SDL_pnglite PRIVATE ${PKG_SDL2_LIBRARIES} ${PKG_ZLIB_LIBRARIES})

if(BUILD_SHARED_LIBS)
  install(TARGETS SDL_pnglite LIBRARY DESTINATION lib)
else()
  install(TARGETS SDL_pnglite ARCHIVE DESTINATION lib)
endif()
install(FILES  SDL_pnglite.h DESTINATION include/SDL2)

option(BUILD_TEST_SUITE "Build test-suite" ${HAVE_SDLIMAGE2})
if (BUILD_TEST_SUITE)
  add_executable(pnglite-test test-suite.c)
  target_link_libraries(pnglite-test PRIVATE SDL_pnglite ${PKG_SDL2_LIBRARIES} ${PKG_SDL2IMAGE_LIBRARIES} ${PKG_ZLIB_LIBRARIES})
  if (MSVC) # hmm. does ming32-w64 fall into this trap too?
    set_target_properties(pnglite-test PROPERTIES LINK_FLAGS "setargv.obj")
  endif(MSVC)
endif(BUILD_TEST_SUITE)

if(NOT (WINDOWS OR CYGWIN))
  if(FREEBSD)
    # FreeBSD uses ${PREFIX}/libdata/pkgconfig
    install(FILES ${SDL_PNGLITE_BINARY_DIR}/sdl_pnglite.pc DESTINATION "libdata/pkgconfig")
  else()
    install(FILES ${SDL_PNGLITE_BINARY_DIR}/sdl_pnglite.pc
      DESTINATION "lib${LIB_SUFFIX}/pkgconfig")
  endif()
endif()
