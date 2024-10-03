-- VulkanEngine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/%{prj.name}/vendor/GLFW/include/"
IncludeDir["tinyobjloader"] = "%{wks.location}/%{prj.name}/vendor/tinyobjloader/"
IncludeDir["glm"] = "%{wks.location}/%{prj.name}/vendor/glm/"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include/"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib/"
Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"