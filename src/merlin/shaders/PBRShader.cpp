#include "pch.h"
#include "merlin/shaders/PBRShader.h"

namespace Merlin {

	shared<PBRShader> PBRShader::create(const std::string& name,
		const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path, bool compile) {

		return createShared<PBRShader>(name, vertex_file_path, fragment_file_path, geometry_file_path, compile);
	}

	PBRShader::PBRShader() : Shader() {}
	PBRShader::PBRShader(std::string n) : Shader(n) {

		supportEnvironment(true);
		supportLights(true);
		supportMaterial(true);
		supportShadows(true);
		supportTexture(true);
	}


	PBRShader::PBRShader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath, bool compile) : Shader(n, vpath, fpath, gpath) {

		supportEnvironment(true);
		supportLights(true);
		supportMaterial(true);
		supportShadows(true);
		supportTexture(true);
	}

}