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

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Flappy-Bird/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Flappy-Bird/vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/Flappy-Bird/vendor/glm"
IncludeDir["ImGui"] = "%{wks.location}/Flappy-Bird/vendor/imgui"

group "Dependencies"
  include "Flappy-Bird/vendor/GLFW"
  include "Flappy-Bird/vendor/Glad"
  include "Flappy-Bird/vendor/imgui"
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
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.Glad}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.ImGui}"
  }

  links
  {
    "GLFW",
    "Glad",
    "opengl32.lib",
    "ImGui"
  }

  filter "system:windows"
    systemversion "latest"

    defines
    {
    }

  filter "configurations:Debug"
    defines
    {
      "WG_DEBUG"
    }
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    defines
    {
      "WG_RELEASE"
    }
    runtime "Release"
    optimize "on"

  filter "configurations:Dist"
    defines
    {
      "WG_DIST"
    }
    runtime "Release"
    optimize "on"

