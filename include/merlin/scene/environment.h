#pragma once
#include "merlin/core/core.h"
#include "merlin/textures/texture.h"
#include "merlin/textures/cubeMap.h"
#include "merlin/graphics/renderableObject.h"

namespace Merlin {
	//https://matheowis.github.io/HDRI-to-CubeMap/
	class Environment : public RenderableObject {
	public:
		Environment(std::string name, GLuint resolution = 512);

		// Setup methods for different textures
		//void loadHDR(const std::string& path);
		void detach() const;
		void attach(Shader& shader) const;
		void draw() const;

		inline bool hasShader() const { return m_shader != nullptr; }
		inline const Shader& getShader() const { return *m_shader; }
		inline void setShader(shared<Shader> shader) { m_shader = shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }

		inline void setGradientColor(const glm::vec3& c) { m_gradient_color = c; }
		const glm::vec3& gradientColor() const { return m_gradient_color; }

		inline void setCubeMap(shared<CubeMap> cm) { m_skybox = cm; }

	private:
		void setupMesh();

		VAO m_vao;
		std::string m_shaderName = "default";
		shared<Shader> m_shader;
		shared<Texture2D> raw_hdri = nullptr;

		GLuint m_resolution = 512;

		glm::vec3 m_gradient_color = glm::vec3(1);
		shared<CubeMap> m_skybox = nullptr;
		shared<CubeMap> m_irradiance = nullptr;

		inline static shared<Shader> conversionShader = nullptr;
		inline static shared<Shader> convolutionShader = nullptr;
	};
	typedef shared<Environment> Environment_Ptr;
}
