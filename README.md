# Remeber
before running the project, you should specify the vulkun path in the addition include directory of config property.

# RayTracing

RayTracing is a simple application framework built with Dear ImGui and designed to be used with Vulkan - basically this means you can seemlessly blend real-time Vulkan rendering with a great UI library to build desktop applications. The plan is to expand RayTracing to include common utilities to make immediate-mode desktop apps and simple Vulkan applications.

Currently supports Windows - with macOS and Linux support planned. Setup scripts support Visual Studio 2022 by default.

![RayTracingExample](https://hazelengine.com/images/ForestLauncherScreenshot.jpg)
_<center>Forest Launcher - an application made with RayTracing</center>_

## Requirements
- [Visual Studio 2022](https://visualstudio.com) (not strictly required, however included setup scripts only support this)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) (preferably a recent version)

## Getting Started
Once you've cloned, run `scripts/Setup.bat` to generate Visual Studio 2022 solution/project files. Once you've opened the solution, you can run the RayTracingApp project to see a basic example (code in `RayTracingApp.cpp`). I recommend modifying that RayTracingApp project to create your own application, as everything should be setup and ready to go.

### 3rd party libaries
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm) (included for convenience)

### Additional
- RayTracing uses the [Roboto](https://fonts.google.com/specimen/Roboto) font ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))
