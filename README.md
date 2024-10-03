# 🏺Interactive 3D Renderer

An interactive application and rendering engine for Windows. The repository contains all of the code for this project, including branches of different use cases.

Code heavily guided by [Brendan Galea](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR)'s series, and build heavily inspired by [Hazel](https://github.com/TheCherno/Hazel)

## Getting Started
Visual Studio 2022 and Windows is recommended to view and run the code.

<ins>1. Downloading the repository:</ins>

Start by cloning the repository with `git clone --recursive https://github.com/raphaeleg/VulkanTriangle`.

If the repository was cloned non-recursively previously, use git submodule update --init to clone the necessary submodules.

<ins>2. Configuring the dependencies:</ins>

1. Run the [Setup.bat](https://github.com/raphaeleg/VkRenderer/blob/main/scripts/Setup.bat) file found in `scripts` folder. This will download the required prerequisites for the project if they are not present yet.
2. One prerequisite is the Vulkan SDK. If it is not installed, the script will execute the `VulkanSDK.exe` file, and will prompt the user to install the SDK.
3. After installation, run the [Setup.bat](https://github.com/raphaeleg/VkRenderer/blob/main/scripts/Setup.bat) file again. If the Vulkan SDK is installed properly, it will then download the Vulkan SDK Debug libraries. (This may take a longer amount of time)
4. After downloading and unzipping the files, the [Win-GenProjects.bat](https://github.com/raphaeleg/VkRenderer/blob/main/scripts/Win-GenProjects.bat) script file will get executed automatically, which will then generate a Visual Studio solution file for user's usage.

If changes are made, or if you want to regenerate project files, rerun the [Win-GenProjects.bat](https://github.com/raphaeleg/VkRenderer/blob/main/scripts/Win-GenProjects.bat) script file found in `scripts` folder.

## Brief Project Structure
<img width="800" alt="General Structure" src="https://github.com/user-attachments/assets/2c27743a-4dbb-4d3a-8a5b-53dfeaf71364">

## Features
![Preview](https://github.com/user-attachments/assets/a4683e87-9b06-4a61-a10f-6693794b5260)
- A fully working 3D Renderer with the proper Vulkan Setup (Window, Pipeline, Buffers, etc...)
- Interactive Camera
- Loading of 3D Models (using [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader))
- Light interaction (phong shading)
- Multiple light sources
- Billboarding
- Alpha Blending
- Premake Project Generation for VS2022

<ins>❗To be implemented</ins>
- Error Handling for shader files when building
- Project Compatibility with MacOS
- Switch to Quaternion Rotations
- Better Inputs
