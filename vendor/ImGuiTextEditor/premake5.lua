project "ImGuiTextEditor"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
    staticruntime "on"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files { 
		"*.h",
		"*.cpp"
	}
	
	vpaths {
	   ["Sources/*"] = {"**.c", "**.cpp", "**.h"}
	}

	IT_IncludeDir = {}
	IT_IncludeDir["imgui"] = "../imgui/"
	
	includedirs	{
		"%{IT_IncludeDir.imgui}"
	}

	links 
	{ 
		"imgui"
	}
	
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

	