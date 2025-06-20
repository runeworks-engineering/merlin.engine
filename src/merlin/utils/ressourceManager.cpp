#include "pch.h"
#include "merlin/core/core.h"
#include "merlin/utils/ressourceManager.h"
#include "merlin/utils/util.h"

namespace Merlin {


	ShaderLibrary::ShaderLibrary() {
		LoadDefaultShaders();
	}

	void ShaderLibrary::LoadDefaultShaders() {
		add(PhongShader::create("default.phong", "assets/common/shaders/graphics/default.model.vert", "assets/common/shaders/graphics/default.model.frag", "assets/common/shaders/graphics/default.model.geom"));
		add(PhongShader::create("instanced.phong", "assets/common/shaders/graphics/instanced.model.vert", "assets/common/shaders/graphics/default.model.frag", "assets/common/shaders/graphics/default.model.geom"));
		add(Shader::create("instanced.sprite", "assets/common/shaders/graphics/instanced.sprite.vert", "assets/common/shaders/graphics/instanced.sprite.frag"));
		add(Shader::create("shadow.depth", "assets/common/shaders/graphics/shadow.depth.vert", "assets/common/shaders/graphics/shadow.depth.frag"));
		add(Shader::create("shadow.omni", "assets/common/shaders/graphics/shadow.omni.vert", "assets/common/shaders/graphics/shadow.omni.frag", "assets/common/shaders/graphics/shadow.omni.geom"));
		add(Shader::create("default.light", "assets/common/shaders/graphics/default.light.vert", "assets/common/shaders/graphics/default.light.frag"));
		//add(Shader::create("default.pbr", "assets/common/shaders/pbr.model.vert", "assets/common/shaders/pbr.model.frag"));
		add(Shader::create("screen.space", "assets/common/shaders/graphics/screen.space.vert", "assets/common/shaders/graphics/screen.space.frag"));
		add(Shader::create("panorama_to_cubemap", "assets/common/shaders/graphics/fullscreen.vert", "assets/common/shaders/graphics/panorama_to_cubemap.frag"));
		//add(Shader::create("isosurface", "assets/common/shaders/isosurface.vert", "assets/common/shaders/graphics/isosurface.frag", "assets/common/shaders/graphics/isosurface.geom"));

		auto envShader = Shader::create("default.skybox", "assets/common/shaders/graphics/default.skybox.vert", "assets/common/shaders/graphics/default.skybox.frag");
		envShader->supportEnvironment(true);
		envShader->supportTexture(true);
		add(envShader);

		shared<Shader> sh = Shader::create("debug.normals", "assets/common/shaders/graphics/debug.normals.vert", "assets/common/shaders/graphics/debug.normals.frag", "assets/common/shaders/graphics/debug.normals.geom");
		add(sh);

	}

	StagedComputeShader_Ptr ComputeShaderLibrary::getStagedComputeShader(const std::string& key){
		ComputeShader_Ptr shader = get(key);
		if (!shader) return nullptr;
		return std::dynamic_pointer_cast<StagedComputeShader>(shader);
	}

	ComputeShaderLibrary::ComputeShaderLibrary() {
		LoadDefaultShaders();
	}

	void ComputeShaderLibrary::LoadDefaultShaders() {
		add(StagedComputeShader::create("prefix.sum", "assets/common/shaders/physics/prefix.sum.comp", 4, false));
		add(StagedComputeShader::create("solver", "assets/common/shaders/physics/solver.comp", 9, false));
		add(ComputeShader::create("isoGen", "assets/common/shaders/physics/isoGen.comp", false));
		add(ComputeShader::create("sectionGen", "assets/common/shaders/physics/texturePlot.comp", false));
	}

	MaterialLibrary::MaterialLibrary() {
		LoadDefaultMaterials();
	}

	void MaterialLibrary::LoadDefaultMaterials() {

		shared<PhongMaterial> defaultMaterial = createShared<PhongMaterial>("default");
		defaultMaterial->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		defaultMaterial->setDiffuse(glm::vec3(0.55, 0.55, 0.55));
		defaultMaterial->setSpecular(glm::vec3(0.70, 0.70, 0.70));
		defaultMaterial->setShininess(0.25);

		shared<PhongMaterial> defaultWhite = createShared<PhongMaterial>("white");
		defaultWhite->setAmbient(glm::vec3(1.0, 1.0, 1.0));
		defaultWhite->setDiffuse(glm::vec3(0.55, 0.55, 0.55));
		defaultWhite->setSpecular(glm::vec3(0.70, 0.70, 0.70));
		defaultWhite->setShininess(0.25);

		shared<PBRMaterial> aluminum = createShared<PBRMaterial>("aluminum");
		aluminum->setAlbedoColor(glm::vec3(0.912, 0.914, 0.920));
		aluminum->setMetallic(1);
		aluminum->setRoughness(0.0);
		aluminum->setAO(0.6);

		shared<PhongMaterial> emerald = createShared<PhongMaterial>("emerald");
		emerald->setAmbient(glm::vec3(0.0215, 0.1745, 0.0215));
		emerald->setDiffuse(glm::vec3(0.07568, 0.61424, 0.07568));
		emerald->setSpecular(glm::vec3(0.633, 0.727811, 0.633));
		emerald->setShininess(0.6);

		shared<PhongMaterial> jade = createShared<PhongMaterial>("jade");
		jade->setAmbient(glm::vec3(0.135, 0.2225, 0.1575));
		jade->setDiffuse(glm::vec3(0.54, 0.89, 0.63));
		jade->setSpecular(glm::vec3(0.316228, 0.316228, 0.316228));
		jade->setShininess(0.1);

		shared<PhongMaterial> obsidian = createShared<PhongMaterial>("obsidian");
		obsidian->setAmbient(glm::vec3(0.05375, 0.05, 0.06625));
		obsidian->setDiffuse(glm::vec3(0.18275, 0.17, 0.22525));
		obsidian->setSpecular(glm::vec3(0.332741, 0.328634, 0.346435));
		obsidian->setShininess(0.3);

		shared<PhongMaterial> pearl = createShared<PhongMaterial>("pearl");
		pearl->setAmbient(glm::vec3(0.25, 0.20725, 0.20725));
		pearl->setDiffuse(glm::vec3(1, 0.829, 0.829));
		pearl->setSpecular(glm::vec3(0.296648, 0.296648, 0.296648));
		pearl->setShininess(0.088);

		shared<PhongMaterial> ruby = createShared<PhongMaterial>("ruby");
		ruby->setAmbient(glm::vec3(0.1745, 0.01175, 0.01175));
		ruby->setDiffuse(glm::vec3(0.61424, 0.04136, 0.04136));
		ruby->setSpecular(glm::vec3(0.727811, 0.626959, 0.626959));
		ruby->setShininess(0.6);

		shared<PhongMaterial> turquoise = createShared<PhongMaterial>("turquoise");
		turquoise->setAmbient(glm::vec3(0.1, 0.18725, 0.1745));
		turquoise->setDiffuse(glm::vec3(0.396, 0.74151, 0.69102));
		turquoise->setSpecular(glm::vec3(0.297254, 0.30829, 0.306678));
		turquoise->setShininess(0.1);

		shared<PhongMaterial> brass = createShared<PhongMaterial>("brass");
		brass->setAmbient(glm::vec3(0.329412, 0.223529, 0.027451));
		brass->setDiffuse(glm::vec3(0.780392, 0.568627, 0.113725));
		brass->setSpecular(glm::vec3(0.992157, 0.941176, 0.807843));
		brass->setShininess(0.21794872);

		shared<PhongMaterial> bronze = createShared<PhongMaterial>("bronze");
		bronze->setAmbient(glm::vec3(0.2125, 0.1275, 0.054));
		bronze->setDiffuse(glm::vec3(0.714, 0.4284, 0.18144));
		bronze->setSpecular(glm::vec3(0.393548, 0.271906, 0.166721));
		bronze->setShininess(0.2);

		shared<PhongMaterial> chrome = createShared<PhongMaterial>("chrome");
		chrome->setAmbient(glm::vec3(0.25, 0.25, 0.25));
		chrome->setDiffuse(glm::vec3(0.4, 0.4, 0.4));
		chrome->setSpecular(glm::vec3(0.774597, 0.774597, 0.774597));
		chrome->setShininess(0.6);

		shared<PhongMaterial> copper = createShared<PhongMaterial>("copper");
		copper->setAmbient(glm::vec3(0.19125, 0.0735, 0.0225));
		copper->setDiffuse(glm::vec3(0.7038, 0.27048, 0.0828));
		copper->setSpecular(glm::vec3(0.256777, 0.137622, 0.086014));
		copper->setShininess(0.1);

		shared<PhongMaterial> gold = createShared<PhongMaterial>("gold");
		gold->setAmbient(glm::vec3(0.24725, 0.1995, 0.0745));
		gold->setDiffuse(glm::vec3(0.75164, 0.60648, 0.22648));
		gold->setSpecular(glm::vec3(0.628281, 0.555802, 0.366065));
		gold->setShininess(0.4);

		shared<PhongMaterial> silver = createShared<PhongMaterial>("silver");
		silver->setAmbient(glm::vec3(0.19225, 0.19225, 0.19225));
		silver->setDiffuse(glm::vec3(0.50754, 0.50754, 0.50754));
		silver->setSpecular(glm::vec3(0.508273, 0.508273, 0.508273));
		silver->setShininess(0.4);

		shared<PhongMaterial> black_plastic = createShared<PhongMaterial>("black plastic");
		black_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		black_plastic->setDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_plastic->setSpecular(glm::vec3(0.50, 0.50, 0.50));
		black_plastic->setShininess(0.25);

		shared<PhongMaterial> cyan_plastic = createShared<PhongMaterial>("cyan plastic");
		cyan_plastic->setAmbient(glm::vec3(0.0, 0.1, 0.06));
		cyan_plastic->setDiffuse(glm::vec3(0.0, 0.50980392, 0.50980392));
		cyan_plastic->setSpecular(glm::vec3(0.50196078, 0.50196078, 0.50196078));
		cyan_plastic->setShininess(0.25);

		shared<PhongMaterial> green_plastic = createShared<PhongMaterial>("green plastic");
		green_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		green_plastic->setDiffuse(glm::vec3(0.1, 0.35, 0.1));
		green_plastic->setSpecular(glm::vec3(0.45, 0.55, 0.45));
		green_plastic->setShininess(0.25);

		shared<PhongMaterial> red_plastic = createShared<PhongMaterial>("red plastic");
		red_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		red_plastic->setDiffuse(glm::vec3(0.5, 0.0, 0.0));
		red_plastic->setSpecular(glm::vec3(0.7, 0.6, 0.6));
		red_plastic->setShininess(0.25);

		shared<PhongMaterial> gray_plastic = createShared<PhongMaterial>("gray plastic");
		gray_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		gray_plastic->setDiffuse(glm::vec3(0.25, 0.25, 0.25));
		gray_plastic->setSpecular(glm::vec3(0.60, 0.60, 0.60));
		gray_plastic->setShininess(0.25);

		shared<PhongMaterial> white_plastic = createShared<PhongMaterial>("white plastic");
		white_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		white_plastic->setDiffuse(glm::vec3(0.55, 0.55, 0.55));
		white_plastic->setSpecular(glm::vec3(0.70, 0.70, 0.70));
		white_plastic->setShininess(0.25);

		shared<PhongMaterial> yellow_plastic = createShared<PhongMaterial>("yellow plastic");
		yellow_plastic->setAmbient(glm::vec3(0.0, 0.0, 0.0));
		yellow_plastic->setDiffuse(glm::vec3(0.5, 0.5, 0.0));
		yellow_plastic->setSpecular(glm::vec3(0.60, 0.60, 0.50));
		yellow_plastic->setShininess(0.25);

		shared<PhongMaterial> black_rubber = createShared<PhongMaterial>("black rubber");
		black_rubber->setAmbient(glm::vec3(0.02, 0.02, 0.02));
		black_rubber->setDiffuse(glm::vec3(0.01, 0.01, 0.01));
		black_rubber->setSpecular(glm::vec3(0.4, 0.4, 0.4));
		black_rubber->setShininess(0.078125);

		shared<PhongMaterial> cyan_rubber = createShared<PhongMaterial>("cyan rubber");
		cyan_rubber->setAmbient(glm::vec3(0.0, 0.05, 0.05));
		cyan_rubber->setDiffuse(glm::vec3(0.4, 0.5, 0.5));
		cyan_rubber->setSpecular(glm::vec3(0.04, 0.7, 0.7));
		cyan_rubber->setShininess(0.078125);

		shared<PhongMaterial> blue_rubber = createShared<PhongMaterial>("blue rubber");
		blue_rubber->setAmbient(glm::vec3(0.0, 0.00, 0.05));
		blue_rubber->setDiffuse(glm::vec3(0.3, 0.3, 0.5));
		blue_rubber->setSpecular(glm::vec3(0.04, 0.04, 0.7));
		blue_rubber->setShininess(0.078125);

		shared<PhongMaterial> green_rubber = createShared<PhongMaterial>("green rubber");
		green_rubber->setAmbient(glm::vec3(0.0, 0.05, 0.0));
		green_rubber->setDiffuse(glm::vec3(0.4, 0.5, 0.4));
		green_rubber->setSpecular(glm::vec3(0.04, 0.7, 0.04));
		green_rubber->setShininess(0.078125);

		shared<PhongMaterial> red_rubber = createShared<PhongMaterial>("red rubber");
		red_rubber->setAmbient(glm::vec3(0.05, 0.0, 0.0));
		red_rubber->setDiffuse(glm::vec3(0.5, 0.4, 0.4));
		red_rubber->setSpecular(glm::vec3(0.7, 0.04, 0.04));
		red_rubber->setShininess(0.078125);

		shared<PhongMaterial> white_rubber = createShared<PhongMaterial>("white rubber");
		white_rubber->setAmbient(glm::vec3(0.05, 0.05, 0.05));
		white_rubber->setDiffuse(glm::vec3(0.5, 0.5, 0.5));
		white_rubber->setSpecular(glm::vec3(0.7, 0.7, 0.7));
		white_rubber->setShininess(0.078125);

		shared<PhongMaterial> yellow_rubber = createShared<PhongMaterial>("yellow rubber");
		yellow_rubber->setAmbient(glm::vec3(0.05, 0.05, 0.0));
		yellow_rubber->setDiffuse(glm::vec3(0.5, 0.5, 0.4));
		yellow_rubber->setSpecular(glm::vec3(0.7, 0.7, 0.04));
		yellow_rubber->setShininess(0.078125);

		shared<PhongMaterial> water = createShared<PhongMaterial>("water");
		water->setAmbient(glm::vec3(91 / 255.0, 178 / 255.0, 244 / 255.0));
		water->setDiffuse(glm::vec3(91 / 255.0, 178 / 255.0, 244 / 255.0));
		water->setSpecular(glm::vec3(91 / 255.0, 178 / 255.0, 244 / 255.0));
		water->setShininess(0.8);
		water->setAlphaBlending(0.1);

		shared<PhongMaterial> glass = createShared<PhongMaterial>("glass");
		water->setAmbient(glm::vec3(80 / 255.0, 80 / 255.0, 80 / 255.0));
		water->setDiffuse(glm::vec3(150 / 255.0, 150 / 255.0, 150 / 255.0));
		water->setSpecular(glm::vec3(1.0, 1.0, 1.0));
		water->setShininess(0.95);
		water->setAlphaBlending(0.4);
		
		add(defaultMaterial);
		add(defaultWhite);
		add(aluminum);
		add(emerald);
		add(water);
		add(jade);
		add(obsidian);
		add(pearl);
		add(ruby);
		add(turquoise);
		add(brass);
		add(bronze);
		add(chrome);
		add(copper);
		add(gold);
		add(silver);
		add(black_plastic);
		add(cyan_plastic);
		add(green_plastic);
		add(red_plastic);
		add(gray_plastic);
		add(white_plastic);
		add(yellow_plastic);
		add(black_rubber);
		add(cyan_rubber);
		add(blue_rubber);
		add(green_rubber);
		add(red_rubber);
		add(white_rubber);
		add(yellow_rubber);


	}


}