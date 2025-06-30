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
	   ["Headers/*"] = "**.h",
	   ["Sources/*"] = {"**.c", "**.cpp"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}
	
	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	MRL_IncludeDir = {}
	MRL_IncludeDir["self"] = "include"
	MRL_IncludeDir["glfw"] = "vendor/glfw/include"
	MRL_IncludeDir["glad"] = "vendor/glad/include"
	MRL_IncludeDir["imgui"] = "vendor/imgui"
	MRL_IncludeDir["glm"] = "vendor/glm"
	MRL_IncludeDir["stb_image"] = "vendor/stb_image"
	MRL_IncludeDir["tinyfiledialogs"] = "vendor/tinyfiledialogs"
	MRL_IncludeDir["tinyxml2"] = "vendor/tinyxml2"
	MRL_IncludeDir["ImGuiTextEditor"] = "vendor/ImGuiTextEditor"
	MRL_IncludeDir["assimp"] = "vendor/assimp/include"


	includedirs
	{
		"%{MRL_IncludeDir.self}",
		"%{MRL_IncludeDir.glfw}",
		"%{MRL_IncludeDir.glad}",
		"%{MRL_IncludeDir.imgui}",
		"%{MRL_IncludeDir.glm}",
		"%{MRL_IncludeDir.stb_image}",
		"%{MRL_IncludeDir.tinyfiledialogs}",
		"%{MRL_IncludeDir.tinyxml2}",
		"%{MRL_IncludeDir.ImGuiTextEditor}",
		"%{MRL_IncludeDir.assimp}"
	}

	links 
	{ 
		"glfw",
		"glad",
		"imgui",
		"stb_image",
		"tinyfiledialogs",
		"tinyxml2",
		"ImGuiTextEditor",
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

