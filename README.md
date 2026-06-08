# Omnigine

A nice and simple game framework built with [raylib](https://www.raylib.com/) and C++.

Omnigine is inspired by [libGDX](https://libgdx.com/), it serves as a tool to have fun making games quickly in a code-only environment!

This repo is a base project template, which builds with [CMake](https://cmake.org).

## Features

* **raylib**: Provides awesome and easy to use graphical, audio, input, and math utilities.
* **Quick Startup**: Simply clone the repo and build with CMake, all necessary external libraries will be downloaded and configured for you!
* **Callback System**: Instead of coding your window initialization and game-loop from scratch, use the callback functions from [omni_main.h](src/omnigine/omni_main.h).
* **Scene System**: Split your games and/or applications into [scenes](src/omnigine/omni_scene.h).
* **Tick System**: Create a tick system callback anywhere in seconds via the [OMNI_TICK](src/omnigine/omni_tick.h) macro.
* **Asset Manager**: One place to quickly load, store, retrieve, and unload all your game's assets.

## Getting Started

The Omnigine repo is a project template that is ready to use.

1. Create a repository with Omnigine as the template.
2. Open the repository in your IDE (Visual Studio Code, CLion, etc.).
3. Build the project with CMake by following the guide below.

### CMake Usage

In [CMakeLists.txt](CMakeLists.txt), you should set a custom name for your project by changing the `project(omnigine)` on line 2 to `project(YOUR_PROJECT_NAME)`.

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

#### Web

Compiling for the web requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

``` bash
mkdir build
cd build
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"
emmake make
```

## Basic Example

This is a basic Omnigine example, it creates a 1280x720 window with a black background and draws the text
`"Hello World!"` in the top-left corner of the screen.
```cpp
#include "omnigine/omni_main.h"

void Init() {
}

bool Render(float dt) {
    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello World!", 0, 0, 52, WHITE);
    EndDrawing();
    return true;
}

void Dispose() {
}
```

## Examples (Work in Progress)

## Documentation (Work in Progress)