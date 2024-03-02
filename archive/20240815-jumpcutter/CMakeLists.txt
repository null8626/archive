cmake_minimum_required(VERSION 3.26.0)

project(
  jumpcutter
  LANGUAGES C
  HOMEPAGE_URL "https://github.com/null8626/jumpcutter"
  DESCRIPTION "Streaming C fork of carykh's Jumpcutter"
)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug)
endif()

file(GLOB JUMPCUTTER_SOURCE_FILES ${CMAKE_SOURCE_DIR}/*.c)

add_executable(jumpcutter ${JUMPCUTTER_SOURCE_FILES})
set_target_properties(jumpcutter PROPERTIES OUTPUT_NAME jumpcutter)

if(WIN32 AND NOT EXISTS ${CMAKE_SOURCE_DIR}/deps)
execute_process(COMMAND powershell -NoLogo -NoProfile -NonInteractive -File install_ffmpeg.ps1 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

file(GLOB FFMPEG_LIBS deps/*.lib)
file(GLOB FFMPEG_DLLS deps/*.dll)

if(MSVC)
target_compile_options(jumpcutter PUBLIC $<$<CONFIG:Debug>:/diagnostics:caret /MTd> $<$<CONFIG:Release>:/MT /O2 /Oi /Oy /Gy>)
else()
target_compile_options(jumpcutter PUBLIC $<$<CONFIG:Release>:-O3> -Wall -Wextra -Wpedantic -Wformat=2 -Wnull-dereference -Wuninitialized -Wdeprecated)
endif()

target_include_directories(jumpcutter PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(jumpcutter ${FFMPEG_LIBS})

if(WIN32)
foreach(FFMPEG_DLL ${FFMPEG_DLLS})
add_custom_command(
  TARGET jumpcutter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${FFMPEG_DLL} $<TARGET_FILE_DIR:jumpcutter>
  COMMAND_EXPAND_LISTS
)
endforeach()
endif()