workspace "Wallpaper-Games"
	architecture "x86_64"
	startproject "Flappy-Bird"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
OPENCV_DIR = os.getenv("OPENCV_DIR")

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "%{wks.location}/Flappy-Bird/vendor/Glad/include"

group "Dependencies"
	include "Flappy-Bird/vendor/Glad"
group ""

project "Flappy-Bird"
	location "Flappy-Bird"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Flappy-Bird/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.Glad}",
		"%{OPENCV_DIR}/include/"
	}

	links 
	{
		"Glad",
		"opengl32.lib",
		"%{OPENCV_DIR}/x64/vc15/lib/*"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

	filter "configurations:Debug"
		defines
		{
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines
		{
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines
		{
		}
		runtime "Release"
		optimize "on"

