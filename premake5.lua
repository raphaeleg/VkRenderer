include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "VulkanEngine"
   architecture "x64"
   configurations { "Debug", "Release" }
   solution_items{ ".editorconfig" }
   
   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

   group "Dependencies"
      include "VulkanEngine/vendor/glfw"
   group ""

   group "Core"
      include "VulkanEngine"
   group ""