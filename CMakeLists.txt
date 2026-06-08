cmake_minimum_required(VERSION 3.16) # FetchContent on 3.11+, SDL3 on 3.16+

# Specify C++17
set(CMAKE_CXX_STANDARD 17)

# Initialize with C and C++ globally
project(omnigine LANGUAGES C CXX)

# Enable Objective-C only on MacOS (fixes CMake error from SDL3)
if (APPLE)
    enable_language(OBJC OBJCXX)
endif()

# Generate compile_commands.json (useful for IDEs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Raylib Dependencies (soon to be replaced by SDL3)
set(RAYLIB_VERSION 6.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

# SDL3 Dependencies
set(SDL3_VERSION 3.4.10)
find_package(SDL3 ${SDL3_VERSION} QUIET)
if (NOT SDL3_FOUND)
  include(FetchContent)
  FetchContent_Declare(
          SDL3
          DOWNLOAD_EXTRACT_TIMESTAMP OFF
          URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-${SDL3_VERSION}.tar.gz
  )
  FetchContent_GetProperties(SDL3)
  if (NOT SDL3_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

# SDL3_image Dependencies
set(SDL3_IMAGE_VERSION 3.4.4)
find_package(SDL3_image ${SDL3_IMAGE_VERSION} QUIET)
if (NOT SDL3_image_FOUND)
  include(FetchContent)
  FetchContent_Declare(
          SDL3_image
          GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
          GIT_TAG release-${SDL3_IMAGE_VERSION}
          GIT_PROGRESS TRUE
          # CMake fetches all submodules by default when using GIT_REPOSITORY
  )
  FetchContent_GetProperties(SDL3_image)
  if (NOT SDL3_image_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(SDL3_image)
  endif()
endif()

# SDL3_ttf Dependencies
set(SDL3_TTF_VERSION 3.2.2)
find_package(SDL3_ttf ${SDL3_TTF_VERSION} QUIET)
if (NOT SDL3_ttf_FOUND)
  include(FetchContent)
  FetchContent_Declare(
          SDL3_ttf
          GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
          GIT_TAG release-${SDL3_TTF_VERSION}
          GIT_PROGRESS TRUE
  )
  FetchContent_GetProperties(SDL3_ttf)
  if (NOT SDL3_ttf_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

# miniaudio Dependencies
set(MINIAUDIO_VERSION 0.11.25)
find_package(miniaudio ${MINIAUDIO_VERSION} QUIET)
if (NOT miniaudio_FOUND)
  include(FetchContent)
  FetchContent_Declare(
          miniaudio
          DOWNLOAD_EXTRACT_TIMESTAMP OFF
          URL https://github.com/mackron/miniaudio/archive/refs/tags/${MINIAUDIO_VERSION}.tar.gz
  )
  FetchContent_GetProperties(miniaudio)
  if (NOT miniaudio_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(miniaudio)
  endif()
endif()

# Our Project
add_executable(${PROJECT_NAME} src/main.cpp
        src/omnigine/omni_main.h
        src/omnigine/omni.h
        src/omnigine/omni_scene.h
        src/omnigine/omni_tick.h
        src/omnigine/omni_assets.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf miniaudio)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ASSERTIONS=1 -s WASM=1")
endif()