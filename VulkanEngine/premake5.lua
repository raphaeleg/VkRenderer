project "VulkanEngine"
   kind "ConsoleApp"
   language "C++"
	cppdialect "C++17"
	staticruntime "off"
   
   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files { "src/**.hpp", "src/**.cpp" }

   includedirs { 
       "%{IncludeDir.VulkanSDK}", 
       "%{IncludeDir.GLFW}", 
       "%{IncludeDir.glm}", 
       "%{IncludeDir.tinyobjloader}", 
   }

   links { "GLFW", "%{Library.Vulkan}" }

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