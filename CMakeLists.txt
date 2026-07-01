cmake_minimum_required(VERSION 3.16) # FetchContent on 3.11+, SDL3 on 3.16+

# Specify C++17
set(CMAKE_CXX_STANDARD 17)

# Initialize with C and C++ globally
project(Omnigine LANGUAGES C CXX)

# Enable Objective-C only on MacOS (fixes CMake error from SDL3)
if (APPLE)
    enable_language(OBJC OBJCXX)
endif()

# Generate compile_commands.json (useful for IDEs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    FetchContent_Populate(miniaudio)

    # Exclude peripheral targets from the default build
    add_subdirectory("${miniaudio_SOURCE_DIR}" "${miniaudio_BINARY_DIR}" EXCLUDE_FROM_ALL)
  endif()
endif()

# Our Project
add_executable(${PROJECT_NAME} src/Main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf miniaudio)
