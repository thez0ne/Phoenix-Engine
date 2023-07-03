# Project Phoenix

![Linux Build](https://github.com/thez0ne/Phoenix-Engine/workflows/Phoenix_Build/badge.svg)
<!-- ![Windows Build](https://github.com/thez0ne/Phoenix-Engine/workflows/Phoenix_Windows/badge.svg) -->

## Building a game engine from scratch uusing Vulkan

---

### Getting Started

To clone with the submodules run ```git clone --recursive```

Make sure to install the [vulkan-sdk](https://vulkan.lunarg.com/sdk/home) before starting development.

This project is setup using [CMake](https://cmake.org/download/).

The project is split into seperate targets:

- Phoenix : Core library for the engine specific code
- Raytracer : Base project, acts as a "game" using the engine code
- PTests : Tests made for the Core lib (STILL NEEDS TO BE SETUP)

---

### How to Run

```bash
mkdir build
cd build
cmake ..
```

After this, based on your compiler you can either just run `make` on Linux with the MinGW compiler or open the generated solution file if using MSVC

---

### Libraries used

These libraries are setup as submodules within this repo

- GLFW - Window Management
- GLM - Mathematics
- spdlog - Logging Library
- Dear ImGUI - UI library to be used in development
- (NEED TO FIND AN IMAGE LOADING SOLUTION)
- (NEED TO FIND AN MODEL LOADING SOLUTION)
