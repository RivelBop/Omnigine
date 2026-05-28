cmake_minimum_required(VERSION 3.24) # FetchContent on 3.11+, OVERRIDE_FIND_PACKAGE on 3.24+
project(omnigine)

# Generate compile_commands.json (useful for IDEs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Always build static libs to avoid missing symbol issues
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Raylib Dependencies
set(RAYLIB_VERSION 6.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

# RRES Dependency
set(RRES_VERSION 1.2.0) # Defines the version of rres to use
find_package(rres ${RRES_VERSION} QUIET) # Searches for rres library on the system
if (NOT rres_FOUND) # If not found
  include(FetchContent) # Includes FetchContent module (a CMake feature for downloading dependencies)
  FetchContent_Declare( # Declares how to fetch rres from GitHub
    rres
    DOWNLOAD_EXTRACT_TIMESTAMP OFF # Prevents re-downloading on same system day
    URL https://github.com/raysan5/rres/archive/refs/tags/${RRES_VERSION}.tar.gz
  )
  FetchContent_GetProperties(rres) # Check if rres was already downloaded, sets rres_POPULATED to 1 if already fetched
  if (NOT rres_POPULATED)
    set(FETCHCONTENT_QUIET NO) # Shows download/build progress in terminal
    FetchContent_MakeAvailable(rres) # Makes rres available to use in your project
  endif()
endif()

# Assimp Dependency (for R3D)
set(ASSIMP_VERSION 6.0.5)
find_package(assimp ${ASSIMP_VERSION} QUIET)
if (NOT assimp_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    assimp
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/assimp/assimp/archive/refs/tags/v${ASSIMP_VERSION}.tar.gz
    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_GetProperties(assimp)
  if (NOT assimp_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(assimp)
  endif()
endif()

# R3D Dependencies
set(R3D_VERSION 0.9.1)

## Setup variables that r3d expects so it actually links them internally
set(R3D_RAYLIB_VENDORED OFF CACHE BOOL "" FORCE)
set(R3D_ASSIMP_VENDORED OFF CACHE BOOL "" FORCE)
set(RAYLIB_LIBRARIES "raylib" CACHE STRING "" FORCE)
set(ASSIMP_LIBRARIES "assimp" CACHE STRING "" FORCE)

find_package(r3d ${R3D_VERSION} QUIET)
if (NOT r3d_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    r3d
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/Bigfoot71/r3d/archive/refs/tags/v${R3D_VERSION}.tar.gz
  )
  FetchContent_GetProperties(r3d)
  if (NOT r3d_POPULATED)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(r3d)
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

# Explicitly tell your project where to find rres.h (rres is header-only, it doesn't have a CMakeLists.txt to build)
if (NOT rres_FOUND)
  # If downloaded, use the fetched source directory
  target_include_directories(${PROJECT_NAME} PRIVATE ${rres_SOURCE_DIR}/src)
else()
  # If found globally, use the system include directories
  target_include_directories(${PROJECT_NAME} PRIVATE ${rres_INCLUDE_DIRS})
endif()

target_include_directories(r3d PRIVATE
  ${raylib_SOURCE_DIR}/src
  ${assimp_SOURCE_DIR}/include
  ${assimp_BINARY_DIR}/include
)
target_link_libraries(r3d PRIVATE assimp)

target_include_directories(${PROJECT_NAME} PRIVATE ${r3d_SOURCE_DIR}/include/r3d)
target_link_libraries(${PROJECT_NAME} raylib assimp r3d)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
