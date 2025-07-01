#include "pch.h"
#include "merlin/graphics/rendererBase.h"
#include "merlin/core/core.h"
#include "merlin/utils/ressourceManager.h"

namespace Merlin {

	RendererBase::RendererBase() : m_currentTransform(glm::mat4(1.0)) {
		backgroundColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	}

	void RendererBase::initialize() {
		resetGlobalTransform();
		enableMultisampling();
		enableSampleShading();
		enableDepthTest();
		enableCubeMap();
		enableFaceCulling();
		m_defaultEnvironment = createShared<Environment>("defaultEnvironment", 16);
		m_defaultAmbient = createShared<AmbientLight>("defaultAmbientLight");

		/**/
		m_defaultDirLight = createShared<DirectionalLight>("light1", glm::vec3(-0.5f, 0.5f, -0.8f));
		m_defaultDirLight->translate(glm::vec3(-10, 0, 0));
		m_defaultDirLight->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
		/**/

		/**/
		m_defaultDirLight2 = createShared<DirectionalLight>("light2", glm::vec3(0.5f, 0.5f, -0.8f));
		m_defaultDirLight2->translate(glm::vec3(-10, 0, 0));
		m_defaultDirLight2->setDiffuse(glm::vec3(1));
		/**/

		/**/
		m_defaultDirLight3 = createShared<DirectionalLight>("light3", glm::vec3(0.0f, -0.5f, -0.8f));
		m_defaultDirLight3->translate(glm::vec3(-10, 0, 0));
		m_defaultDirLight3->setDiffuse(glm::vec3(1));
		/**/

		/**/
		m_defaultAmbient = createShared<AmbientLight>("light4");
		m_defaultAmbient->setAmbient(glm::vec3(0.2));
		/**/
	}

	void RendererBase::clear() {
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererBase::reset() {
		resetMatrix();
		m_activeLights.clear();
		m_scenePoints.clear();
		Texture2D::resetTextureUnits();
		renderToDefault();
	}


	void RendererBase::disableMultisampling() {
		glDisable(GL_MULTISAMPLE);
	}

	void RendererBase::disableTransparency() {
		glDisable(GL_BLEND);
	}

	void RendererBase::enableTransparency() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void RendererBase::disableSampleShading() {
		glDisable(GL_SAMPLE_SHADING);
	}

	void RendererBase::enableSampleShading() {
		glEnable(GL_SAMPLE_SHADING);
		glMinSampleShading(8);
	}

	void RendererBase::enableMultisampling() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LINE_SMOOTH);
	}

	void RendererBase::enableFaceCulling() {
		// Enables Cull Facing
		glEnable(GL_CULL_FACE);
		// Keeps front faces
		glCullFace(GL_BACK);
		// Uses counter clock-wise standard
		glFrontFace(GL_CCW);

		use_culling = true;
	}

	void RendererBase::disableFaceCulling() {
		// Enables Cull Facing
		glDisable(GL_CULL_FACE);
		use_culling = false;
	}

	bool RendererBase::useFaceCulling(){
		return use_culling;
	}

	void RendererBase::enableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::disableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}	
	
	void RendererBase::enableDepthWrite() {
		glDepthMask(GL_TRUE); // Write to depth buffer
	}

	void RendererBase::disableDepthWrite() {
		glDepthMask(GL_FALSE);
	}

	void RendererBase::enableCubeMap() {
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}	
	
	void RendererBase::disableCubeMap() {
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void RendererBase::enableShadows(){
		use_shadows = true;
	}

	void RendererBase::disableShadows() {
		use_shadows = false;
	}

	void RendererBase::disableEnvironment(){
		use_environment = false;
	}

	void RendererBase::enableEnvironment() {
		use_environment = true;
	}

	void RendererBase::hideLights(){
		display_lights = false;
	}

	void RendererBase::showLights(){
		display_lights = true;
	}

	void RendererBase::useDefaultLight(bool state){
		use_default_light = state;
	}

	bool RendererBase::useDefaultLight() {
		return use_default_light;
	}

	void RendererBase::setBackgroundColor(float r, float g, float b, float a) {
		backgroundColor = glm::vec4(r, g, b, a);
	}

	shared<Shader> RendererBase::getShader(std::string n) {
		return ShaderLibrary::instance().get(n);
	}

	shared<MaterialBase> RendererBase::getMaterial(std::string n) {
		return MaterialLibrary::instance().get(n);
	}

	void RendererBase::loadShader(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geomShaderPath) {
		shared<Shader> shader = Shader::create(name, vertexShaderPath, fragmentShaderPath, geomShaderPath);
		ShaderLibrary::instance().add(shader);
	}

	void RendererBase::addMaterial(shared<MaterialBase> material) {
		MaterialLibrary::instance().add(material);
	}

	void RendererBase::addShader(shared<Shader> shader) {
		if (!shader->isCompiled()) Console::error("Renderer") << "Shader is not compiled. Compile the shader before adding them to the ShaderLibrary" << Console::endl;
		ShaderLibrary::instance().add(shader);
	}

	void RendererBase::setRenderModeOveride(RenderMode mode){
		m_renderModeOverride = mode;
	}

	void  RendererBase::renderTo(FBO_Ptr target) {
		m_target = target;
	}

	void RendererBase::renderToDefault() {
		if (m_target) m_target->unbind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_target = nullptr;
	}

	void RendererBase::activateTarget()	{
		if (m_target) {
			m_target->bind();
			glViewport(0,0,m_target->width(), m_target->height());
		}
	}


	void RendererBase::setEnvironmentGradientColor(float r, float g, float b) {
		setEnvironmentGradientColor(glm::vec3(r, g, b));
	}

	void RendererBase::setEnvironmentGradientColor(glm::vec3 color) {
		if (m_currentEnvironment) m_currentEnvironment->setGradientColor(color);
		m_defaultEnvironment->setGradientColor(color);
	}

	void RendererBase::pushMatrix() {
		m_matrixStack.push(m_currentTransform);
	}

	void RendererBase::popMatrix() {
		if (!m_matrixStack.empty()) {
			m_currentTransform = m_matrixStack.top();
			m_matrixStack.pop();
		}
	}

	void RendererBase::resetMatrix() {
		while (!m_matrixStack.empty()) {
			m_matrixStack.pop();
		}
		m_currentTransform = m_globalTransform;
	}


	void RendererBase::gatherLights(const shared<RenderableObject>& object) {
		if (const auto light = std::dynamic_pointer_cast<Light>(object)) {
			light->applyRenderTransform(m_currentTransform);
			m_activeLights.push_back(light);
		}
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			BoundingBox bb = mesh->getBoundingBox();
			glm::vec3 size = bb.max - bb.min;
		}

		for (const auto& child : object->children()) {
			if (!child->isHidden()) gatherLights(child);
		}
	}

	const std::vector<glm::vec3>& RendererBase::getScenePoints() const {
		return m_scenePoints;
	}
	void RendererBase::addPoint(glm::vec3 pt){
		m_scenePoints.push_back(pt);
	}
}