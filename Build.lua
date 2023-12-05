VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "New-Workspace"
    architecture "x86_64"
    configurations { "Debug", "Release" }

project "New-Project"
    kind "ConsoleApp"
    language "C++"

    targetdir "Binaries/%{cfg.system}-%{cfg.buildcfg}"
    objdir "Binaries/%{cfg.system}-%{cfg.buildcfg}/Intermediates"

    files { "Source/**.cpp" }

    filter { "system:Windows", "configurations:Debug" }
        includedirs { "Vendor", "$(VULKAN_SDK)/Include", "Source" }
        links {
            "Vendor/sfml-window-d",
            "Vendor/sfml-system-d",
            "Vendor/sfml-network-d",
            "Vendor/sfml-audio-d",
            "Vendor/openal32",
            "$(VULKAN_SDK)/Lib/vulkan-1"
        }

    filter { "system:Windows", "configurations:Release" }
        includedirs { "Vendor", "$(VULKAN_SDK)/Include", "Source" }
        links {
            "Vendor/sfml-window",
            "Vendor/sfml-system",
            "Vendor/sfml-network",
            "Vendor/sfml-audio",
            "Vendor/openal32",
            "$(VULKAN_SDK)/Lib/vulkan-1"
        }

    filter "system:Linux"
        includedirs { "Source" }
        links {
            "sfml-window",
            "sfml-system",
            "sfml-network",
            "sfml-audio",
            "openal32",
            "vulkan"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"
        optimize "Off"
        defines "DEBUG"

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "On"
        defines "NDEBUG"
