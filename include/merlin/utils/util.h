#pragma once
#include "merlin/core/core.h"
#include "glm/glm.hpp"
#include "merlin/core/log.h"
#include "merlin/graphics/mesh.h"

#define DEG_TO_RAD 0.01745329251994
#define RAD_TO_DEG 57.2957795130823

namespace Merlin::Utils {

	enum class FileType {
		HDR,
		PNG,
		JPG,
		OBJ,
		STL,
		GEOM,
		GLTF,
		UNKNOWN
	};

	FileType getFileType(const std::string& file_path);
	std::string getFileExtension(const std::string& filepath);
	std::string getFileName(const std::string& filepath);
	std::string getFileFolder(const std::string& filepath);

	const std::map<BufferType, GLuint>& getBufferTypes();
	const GLuint getBufferTypeSize(BufferType);//in bytes
	

	template<typename T>
	void debugVector(std::vector<T>& vec);

	std::string get_uuid();


	template<typename T>
	void debugVector(std::vector<T>& vec) {
		Console::info("Vector") << " : " << Console::endl << "[";
		for (GLuint i = 0; i < ((vec.size() > 100) ? 100 : vec.size() - 1); i++) {
			Console::print() << vec[i] << ", ";
		}
		if (vec.size() > 100) Console::print() << "..., ";
		Console::print() << vec[vec.size() - 1] << "]" << Console::endl << Console::endl;

	}
}