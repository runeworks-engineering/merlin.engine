#pragma once
#include "merlin/core/core.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/scene/light.h"
#include "merlin/memory/frameBuffer.h"

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

		void enableDepthWrite();
		void disableDepthWrite();

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

		void gatherLights(const shared<RenderableObject>& object);
		const std::vector<glm::vec3>& getScenePoints() const;
		void addPoint(glm::vec3);

		void setEnvironmentGradientColor(float r, float g, float b);
		void setEnvironmentGradientColor(glm::vec3 color);

		//Matrix stack
		inline void resetGlobalTransform() { m_globalTransform = glm::mat4(1); }
		inline void applyGlobalTransform(glm::mat4 globalTransform) { m_globalTransform = globalTransform; }
		void pushMatrix();
		void popMatrix();
		void resetMatrix();

		//Shader management shortcut
		shared<Shader> getShader(std::string n);
		shared<MaterialBase> getMaterial(std::string n);

		void loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath = "");
		void addMaterial(shared<MaterialBase> material);
		void addShader(shared<Shader> shader);

		void setRenderModeOveride(RenderMode mode);

		void renderTo(FBO_Ptr target);
		void renderToDefault();
		void activateTarget();

	protected:
		bool debug = false;
		
		bool use_culling = true;
		bool use_shadows = true;
		bool use_environment = true;
		bool display_lights = false;
		bool use_default_light = false; //Use has backup if no light set

		//Light
		shared<AmbientLight> m_defaultAmbient;
		shared<DirectionalLight> m_defaultDirLight;
		shared<DirectionalLight> m_defaultDirLight2;
		shared<DirectionalLight> m_defaultDirLight3;

		std::vector<shared<Light>> m_activeLights;

		std::vector<glm::vec3> m_scenePoints;

		RenderMode m_renderMode = RenderMode::LIT;
		RenderMode m_renderModeOverride = RenderMode::DEFAULT;

		//Matrix stack
		glm::mat4 m_globalTransform = glm::mat4(1);
		glm::mat4 m_currentTransform;
		std::stack<glm::mat4> m_matrixStack;

		//Environement
		glm::vec4 backgroundColor;
		shared<Environment> m_defaultEnvironment = nullptr;
		shared<Environment> m_currentEnvironment = nullptr;

		FBO_Ptr m_target = nullptr;
	};
}
