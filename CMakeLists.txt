cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(omnigine)

# Generate compile_commands.json (useful for IDEs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Raylib Dependencies
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

# Our Project
add_executable(${PROJECT_NAME} src/main.cpp
        src/omnigine_main.h
        src/omnigine.h
        src/omnigine_scene.h
        src/omnigine_tick.h
)

# Explicitly tell your project where to find rres.h (rres is header-only, it doesn't have a CMakeLists.txt to build)
if (NOT rres_FOUND)
  # If downloaded, use the fetched source directory
  target_include_directories(${PROJECT_NAME} PRIVATE ${rres_SOURCE_DIR}/src)
else()
  # If found globally, use the system include directories
  target_include_directories(${PROJECT_NAME} PRIVATE ${rres_INCLUDE_DIRS})
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

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
