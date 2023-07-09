# Vulkraft
A C++ Vulkan implementation of Minecraft

## Build 

How to build this project:

### Unix system

* Install vulkan, glm and glfw. You will also need cmake

> If you are using Mac, you need to install Homebrew and MacOS Vulkan SDK

* Go to the directory of the project and run `./unixBuild.sh`

### Windows system

* Install Vulkan sdk

* Install cmake and a compiler such as minGW (be sure to have a 64 bits version)

* Download GLFW and GLM libraries

* Rename envWindows.cmake to .env.cmake and replace the path of the required variables

* If using Visual Studio 2019 run  `mkdir build && cmake -S . -B .\build\`

* If you are using minGW 64 run `./mingwBuild.bat`. Then you can go to build and run `./Vulkraft`.