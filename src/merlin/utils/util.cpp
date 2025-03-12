
#include "pch.h"
#include "merlin/utils/util.h"
#include "merlin/utils/voxelizer.h"

#include <random>

namespace Merlin::Utils {	

	const std::map<std::string, GLuint> m_typeMap = {
		{"int", 4},
		{"float", 4},
		{"uint", 4},
		{"bool", 4},
		{"double", 8},
		{"dvec2", 16},
		{"dvec3", 24},
		{"dvec4", 32},
		{"ivec2", 8},
		{"ivec3", 12},
		{"ivec4", 16},
		{"uvec2", 8},
		{"uvec3", 12},
		{"uvec4", 16},
		{"vec2", 8},
		{"vec3", 12},
		{"vec4", 16},
		{"mat2", 16},
		{"mat2x3", 24},
		{"mat2x4", 32},
		{"mat3x2", 24},
		{"mat3", 36},
		{"mat3x4", 48},
		{"mat4x2", 32},
		{"mat4x3", 48},
		{"mat4", 64}
	};

	FileType getFileType(const std::string& file_path) {
		std::string extension = getFileExtension(file_path);
		if (extension == "obj") {
			return FileType::OBJ;
		}
		else if (extension == "stl") {
			return FileType::STL;
		}
		else if (extension == "geom") {
			return FileType::GEOM;
		}
		else if (extension == "gltf") {
			return FileType::GLTF;
		}
		else if (extension == "png") {
			return FileType::PNG;
		}
		else if (extension == "jpg" || extension == "jpeg") {
			return FileType::JPG;
		}
		else if (extension == "hdr") {
			return FileType::HDR;
		}
		else {
			// Unknown file type
			return FileType::UNKNOWN;
		}
	}

	std::string get_uuid() {
		static std::random_device dev;
		static std::mt19937 rng(dev());

		std::uniform_int_distribution<int> dist(0, 15);

		const char* v = "0123456789abcdef";
		const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

		std::string res;
		for (int i = 0; i < 16; i++) {
			if (dash[i]) res += "-";
			res += v[dist(rng)];
			res += v[dist(rng)];
		}
		return res;
	}

	std::string getFileExtension(const std::string& filepath) {
		size_t pos = filepath.find_last_of(".");
		if (pos == std::string::npos) {
			// No extension found
			return "";
		}
		return filepath.substr(pos + 1);
	}

	std::string getFileName(const std::string& filepath)
	{
		// Find the last occurrence of the directory separator character
		std::size_t pos = filepath.find_last_of("\\/");

		// If the separator was not found, return the entire filepath
		if (pos == std::string::npos) {
			return filepath;
		}

		// Return the part of the filepath after the separator
		return filepath.substr(pos + 1);
	}

	std::string getFileFolder(const std::string& filepath){
		// Find the last occurrence of the directory separator character
		std::size_t pos = filepath.find_last_of("\\/");

		// If the separator was not found, return the entire filepath
		if (pos == std::string::npos) {
			return filepath;
		}

		// Return the part of the filepath of last folder
		return filepath.substr(0, pos+1);
	}

	const std::map<std::string, GLuint>& getGLTypes(){
		return m_typeMap;
	}

	const GLuint getGLTypeSize(const std::string& name){
		if (m_typeMap.find(name) != m_typeMap.end())
			return m_typeMap.at(name);
		else return -1;
	}


}