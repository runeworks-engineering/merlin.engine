workspace "Merlin"
	architecture "x64"
	startproject "merlin.example"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
solutiondir = path.getabsolute("./")

IncludeDir = {}
IncludeDir["self"] = "include"
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glad"] = "vendor/glad/include"
IncludeDir["imgui"] = "vendor/imgui"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb_image"] = "vendor/stb_image"
IncludeDir["tinyfiledialogs"] = "vendor/tinyfiledialogs"
IncludeDir["assimp"] = "vendor/assimp/include"

-- Projects
group "Dependencies"
	include "vendor/glfw"
	include "vendor/glad"
	include "vendor/imgui"
	include "vendor/stb_image"
	include "vendor/tinyfiledialogs"
	include "vendor/assimp"
	
group "Library"
	project "merlin"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files { 
		"include/**.h",
		"src/**.c",
		"src/**.cpp",
		"vendor/imgui/backends/imgui_impl_glfw.*",
		"vendor/imgui/backends/imgui_impl_opengl3.*",
		"assets/shaders/**.*"
	}

	vpaths {
	   ["Sources/*"] = {"**.c", "**.cpp", "**.h"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	includedirs
	{
		"%{IncludeDir.self}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.tinyfiledialogs}",
		"%{IncludeDir.assimp}"
	}

	links 
	{ 
		"glfw",
		"glad",
		"imgui",
		"stb_image",
		"tinyfiledialogs",
		"assimp",
		"opengl32.lib"
	}

	filter { 'files:vendor/imgui/backends/imgui_impl_glfw.cpp' }
		flags { 'NoPCH' }
		
	filter { 'files:vendor/imgui/backends/imgui_impl_opengl3.cpp' }
		flags { 'NoPCH' }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"
		
group "Examples"
	include "examples/merlin.example"
	include "examples/merlin.example2D"
	include "examples/merlin.particles"
	include "examples/merlin.graphics"
	include "examples/merlin.sandbox"
	include "examples/merlin.slicer"
	
