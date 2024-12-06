project "merlin.particles"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	debugdir (solutiondir .."/examples/merlin.examples" .. "/%{prj.name}")
	targetdir (solutiondir .."/bin/" .. outputdir .. "/%{prj.name}")
	objdir (solutiondir .."/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"assets/shaders/**.*"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLM_ENABLE_EXPERIMENTAL"
	}
	
	removefiles { "assets/common/**.*" }

	vpaths {
	   ["Headers/*"] = "**.h",
	   ["Sources/*"] = {"**.c", "**.cpp"},
	   ["Docs"] = "**.md",
	   ["Assets/*"] = "assets/**.*"
	}

	includedirs { solutiondir .. "/vendor/glfw/include" }

	filter { "system:windows" }
		ignoredefaultlibraries { "msvcrt" }

	includedirs
	{
		solutiondir .. "/vendor/spdlog/include",
		solutiondir .. "/src",
		solutiondir .. "/vendor",
		solutiondir .. "/%{IncludeDir.glm}",
		solutiondir .. "/%{IncludeDir.glad}",
		solutiondir .. "/%{IncludeDir.tinyfiledialogs}",
		solutiondir .. "/%{IncludeDir.imgui}"
	}

	links
	{
		"merlin.core",
		"glad",
		"glfw",
		"imgui",
		"tinyfiledialogs",
		"opengl32"
	}



	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLCORE_PLATFORM_WINDOWS",
			"NOMINMAX"
		}

	filter "configurations:Debug"
		defines "GLCORE_DEBUG"
		runtime "Debug"
		symbols "on"
		postbuildcommands {
		  "{COPYDIR} %[assets] %[%{!cfg.targetdir}/assets]",
		  "{COPYDIR} %[" .. solutiondir .. "/assets] %[%{!cfg.debugdir}/assets/common]"
		}

	filter "configurations:Release"
		defines "GLCORE_RELEASE"
		runtime "Release"
		optimize "on"
		postbuildcommands {
		  "{COPYDIR} %[assets] %[%{!cfg.targetdir}/assets]",
		  "{COPYDIR} %[" .. solutiondir .. "/assets] %[%{!cfg.debugdir}/assets/common]"
		}