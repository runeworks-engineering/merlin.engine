#include "pch.h"
#include "merlin/core/core.h"
#include "merlin/graphics/deferredRenderer.h"




namespace Merlin {

	DeferredRenderer::DeferredRenderer() {}

	DeferredRenderer::~DeferredRenderer() {}


	void DeferredRenderer::renderScene(const Scene& scene, const Camera& camera) {
		if (debug)Console::info() << "Rendering scene" << Console::endl;

		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) gatherLights(node);
		}

		if (m_activeLights.size() == 0 || use_default_light) {
			m_activeLights.push_back(m_defaultAmbient);
			m_activeLights.push_back(m_defaultDirLight);
			m_activeLights.push_back(m_defaultDirLight2);
			m_activeLights.push_back(m_defaultDirLight3);
		}

		geometryPass(scene, camera);
		lightingPass(camera);
		finalCompositionPass();
	}

	void DeferredRenderer::renderEnvironment(const Environment& env, const Camera& camera){
		if(debug)Console::info() << "Rendering Environment" << Console::endl;

	}

	void DeferredRenderer::applyPostProcessing()
	{
	}

	void DeferredRenderer::setupGBuffer(int width, int height)
	{
	}

	void DeferredRenderer::bindGBuffer()
	{
	}

	void DeferredRenderer::unbindGBuffer()
	{
	}


	void DeferredRenderer::renderDepth(const shared<RenderableObject>& object, shared<Shader> shader){
		if (debug)Console::info() << "Rendering Depth" << Console::endl;

	}

	void DeferredRenderer::geometryPass(const Scene& scene, const Camera& camera){
		bindGBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		for (const auto& node : scene.nodes()) {
			if (!node->isHidden()) render(node, camera);
		}

		unbindGBuffer();
	}

	void DeferredRenderer::lightingPass(const Camera& camera){

		g_buffer_final->bind();
		glClear(GL_COLOR_BUFFER_BIT);

		m_lightingShader->use();

		// Bind G-buffer textures
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_gPosition);
		

		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, m_gNormal);


		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);

		// If you have extra textures, bind them as well
		// glActiveTexture(GL_TEXTURE3);
		// glBindTexture(GL_TEXTURE_2D, m_gMaterial);

		// Set shader uniforms, e.g. camera info, light info
		// m_lightingShader->setUniform("viewPos", camera.getPosition());

		// Possibly pass an array of lights or do multiple passes
		// for (int i = 0; i < lights.size(); i++) { ... }

		// Render a full-screen quad
		//renderFullScreenQuad();

		g_buffer_final->unbind();

	}

	void DeferredRenderer::finalCompositionPass(){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Possibly a "final composition" shader
		m_postProcessShader->use();

		// Bind the color texture from the lighting pass FBO
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, m_lightingColorBuffer);

		// render a fullscreen quad
		//renderFullScreenQuad();

	}

	void DeferredRenderer::renderLight(const Light& li, const Camera& camera){
		if (debug)Console::info() << "Rendering Light" << Console::endl;

	}

	void DeferredRenderer::renderMesh(const Mesh& mesh, const Camera& camera) {
		if (debug)Console::info() << "Rendering Mesh" << Console::endl;


	}
	
	void DeferredRenderer::renderParticleSystem(const ParticleSystem& ps, const Camera& camera) {

	}

	void DeferredRenderer::renderTransformObject(const TransformObject& obj, const Camera& camera) {
		if (debug)Console::info() << "Rendering TransformObject" << Console::endl;
		render(obj.getXAxisModel(), camera);
		render(obj.getYAxisModel(), camera);
		render(obj.getZAxisModel(), camera);
	}

	void DeferredRenderer::render(const shared<RenderableObject>& object, const Camera& camera) {
		pushMatrix();
		m_currentTransform *= object->transform();

		if (object->isWireFrame()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//The object is a mesh
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(object)) {
			renderMesh(*mesh, camera);
		}//The object is a model
		else if (const auto ps = std::dynamic_pointer_cast<ParticleSystem>(object)) {
			renderParticleSystem(*ps, camera);
		}
		else if (const auto iso = std::dynamic_pointer_cast<IsoSurface>(object)) {
			render(iso->mesh(), camera);
		}
		else if (const auto li = std::dynamic_pointer_cast<Light>(object)) {
			if(display_lights) renderLight(*li, camera);
		}
		else if (const auto model = std::dynamic_pointer_cast<Model>(object)) {
			for (const auto& mesh : model->meshes()) {
				render(mesh, camera);
			}
		}//The object is a scene
		else if (const auto scene = std::dynamic_pointer_cast<Scene>(object)) {
			renderScene(*scene, camera); //Propagate to childrens
		}//The object is a scene node
		else if (const auto ps = std::dynamic_pointer_cast<TransformObject>(object)) {
			renderTransformObject(*ps, camera); //Propagate to childrens
		}

		for (auto node : object->children()) {
			render(node, camera);//Propagate to childrens
		}

		popMatrix();
	}

}