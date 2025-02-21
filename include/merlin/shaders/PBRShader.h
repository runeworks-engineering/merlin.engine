#pragma once
#include "shader.h"


namespace Merlin {
	class PBRShader : public Shader{
	public:
		PBRShader();
		PBRShader(std::string n);
		PBRShader(std::string n,
			const std::string vpath,
			const std::string fpath,
			const std::string gpath = "", bool compile = true);

		static shared<PBRShader> create(const std::string& name,
			const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "", bool compile = true);



	};

	typedef shared<PBRShader> PBRShader_Ptr;
}

