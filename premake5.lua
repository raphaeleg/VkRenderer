workspace "VulkanEngine"
   architecture "x64"
   configurations { "Debug", "Release" }
   
   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "VulkanEngine"
   location "VulkanEngine"
   kind "ConsoleApp"
   language "C++"

   files { "%{prj.name}/src/**.hpp", "%{prj.name}/src/**.cpp" }
   includedirs { 
       "vendor/glfw/include/", 
       "vendor/glm/", 
       "vendor/tinyobjloader/", 
   }
   filter { "system:windows" }
       cppdialect "C++17"
       staticruntime "On"
       systemversion "latest"

   filter { "configurations:Debug" }
      defines { "DEBUG" }
      symbols "On"

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"