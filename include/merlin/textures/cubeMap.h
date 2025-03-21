#pragma once
#include "merlin/core/core.h"
#include "merlin/textures/texture.h"
#include <vector>


namespace Merlin{
	class CubeMap : public TextureBase {
	public:
		CubeMap(TextureType = TextureType::ENVIRONMENT);

		//Settings
		void setInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void setRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER, GLuint _wrapR = GL_CLAMP_TO_BORDER);
		void setBorderColor4f(float colors[4]);
		void setBorderColor4f(float R, float G, float B, float A);
		void generateMipmap() const;

		//Memory
		void allocate(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type);
		void reserve(GLuint width, GLuint height, GLuint depth = 0, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLuint width, GLuint height, GLuint depth = 0) override;

		static shared<CubeMap> create(GLuint width, GLuint height, TextureType = TextureType::ENVIRONMENT);
		static shared<CubeMap> create(const std::vector<std::string>& paths, TextureType = TextureType::ENVIRONMENT);

	private:
		void loadFromFiles(const std::vector<std::string>& faces);


	};
}

