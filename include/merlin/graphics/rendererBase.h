#pragma once
#include "merlin/core/core.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/scene/light.h"

#include <vector>
#include <stack>

namespace Merlin {

	class RendererBase {
	public:
		RendererBase();
		virtual ~RendererBase() = default;

		virtual void reset();
		virtual void clear();
		void setBackgroundColor(float r, float g, float b, float a);

		virtual void initialize();

		void enableTransparency();
		void disableTransparency();

		void enableSampleShading();
		void disableSampleShading();

		void enableMultisampling();
		void disableMultisampling();

		void enableFaceCulling();
		void disableFaceCulling();
		bool useFaceCulling();

		void enableDepthTest();
		void disableDepthTest();

		void enableCubeMap();
		void disableCubeMap();

		void enableShadows();
		void disableShadows();

		void disableEnvironment();
		void enableEnvironment();

		void hideLights();
		void showLights();

		void useDefaultLight(bool state = true);
		bool useDefaultLight();

		void gatherLights(const Shared<RenderableObject>& object);

		void setEnvironmentGradientColor(float r, float g, float b);
		void setEnvironmentGradientColor(glm::vec3 color);

		//Matrix stack
		inline void resetGlobalTransform() { m_globalTransform = glm::mat4(1); }
		inline void applyGlobalTransform(glm::mat4 globalTransform) { m_globalTransform = globalTransform; }
		void pushMatrix();
		void popMatrix();
		void resetMatrix();

		//Shader management shortcut
		Shared<Shader> getShader(std::string n);
		Shared<MaterialBase> getMaterial(std::string n);

		void loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");
		void addMaterial(Shared<MaterialBase> material);
		void addShader(Shared<Shader> shader);


	protected:
		bool debug = false;

		bool use_culling = true;
		bool use_shadows = true;
		bool use_environment = true;
		bool display_lights = false;
		bool use_default_light = false; //Use has backup if no light set

		float m_scene_scale = 1.0;

		//Light
		Shared<AmbientLight> m_defaultAmbient;
		Shared<DirectionalLight> m_defaultDirLight;
		Shared<DirectionalLight> m_defaultDirLight2;
		Shared<DirectionalLight> m_defaultDirLight3;

		std::vector<Shared<Light>> m_activeLights;

		//Matrix stack
		glm::mat4 m_globalTransform = glm::mat4(1);
		glm::mat4 m_currentTransform;
		std::stack<glm::mat4> m_matrixStack;

		//Environement
		glm::vec4 backgroundColor;
		Shared<Environment> m_defaultEnvironment = nullptr;
		Shared<Environment> m_currentEnvironment = nullptr;
	};
}
