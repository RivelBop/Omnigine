# Omnigine

A nice and simple game framework built with [SDL](https://www.libsdl.org/), [miniaudio](https://miniaud.io/), and C++.

Omnigine is inspired by [libGDX](https://libgdx.com/), it serves as a tool to have fun making games quickly in a code-only environment!

This repo is a base project template, which builds with [CMake](https://cmake.org).

## Features

* **SDL**: Provides the necessary application utilities (window, rendering, input, etc.).
* **miniaudio**: Provides the necessary audio-capabilities for wide-range of use.
* **Quick Startup**: Simply clone the repo and build with CMake, all necessary external libraries will be downloaded and configured for you!
* **Callback System**: Instead of coding your window initialization and game-loop from scratch, use the callback functions from [omni_main.h](src/omnigine/omni_main.h).
* **Scene System**: Split your games and/or applications into [scenes](src/omnigine/omni_scene.h).
* **Tick System**: Create a tick system callback anywhere in seconds via the [OMNI_TICK](src/omnigine/omni_tick.h) macro.
* **Camera System**: Create a [camera](src/omnigine/omni_render.h) to project the renderer through.
* **Asset Manager**: Quickly load, store, retrieve, and unload all your game's assets via the [Omni::Assets](src/omnigine/omni_assets.h) class.
* **Atlas Support**: Create and load atlas files made with [GDX Texture Packer](https://github.com/crashinvaders/gdx-texture-packer-gui) via the [Omni::Atlas](src/omnigine/omni_atlas.h) class.

## Getting Started

The Omnigine repo is a project template that is ready to use.

1. Create a repository with Omnigine as the template.
2. Open the repository in your IDE (Visual Studio Code, CLion, etc.).
3. Build the project with CMake by following the guide below.

### CMake Usage

In [CMakeLists.txt](CMakeLists.txt), you should set a custom name for your project by changing the `project(omnigine LANGUAGES C CXX)` to `project(YOUR_PROJECT_NAME LANGUAGES C CXX)`.

Any new source files that you create for your project should be added to the `add_executable` block.

To compile your project use one of the following depending on your build target:

#### Desktop (Windows, Linux, MacOS)

``` bash
cmake -B build
cmake --build build
```

Before running these commands on Windows, make sure to install and setup the following (for AVIF support):

##### NASM Assembler
1. Run `winget install NASM.NASM` in cmd or powershell.
2. Add `C:\Users\%USERNAME%\AppData\Local\bin\NASM` to your user `PATH`.

##### Perl
1. Run `winget install StrawberryPerl.StrawberryPerl` in cmd or powershell.
2. To prevent CMake issues, remove `C:\Strawberry\c\bin` from your system `PATH`.

Before running these commands on Linux, make sure to download the necessary build dependencies by following this [guide](https://wiki.libsdl.org/SDL3/README-linux#build-dependencies).

#### Web

Compiling for the web requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

``` bash
mkdir build
cd build
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
emmake make
```

## Basic Example

This is a basic Omnigine example, it creates a 640x480 window with a black background and draws the text
`Hello World!` in the top-left corner of the screen.
```cpp
#include "omnigine/omni_main.h"

WindowProperties InitWindow(int argc, char *argv[])
{
    return {};
}

bool Init(int argc, char *argv[])
{
    return true;
}

bool Render(float dt)
{
    Omni::Camera camera{ 0, 0, 6.5f };
    Omni::RenderToCamera(&camera);
    Omni::SetRenderColor(255, 255, 255, 255);
    Omni::RenderDebugText(0, 0, "Hello World!");
    return true;
}

void Dispose()
{
}
```

## Examples (Work in Progress)

## Documentation (Work in Progress)