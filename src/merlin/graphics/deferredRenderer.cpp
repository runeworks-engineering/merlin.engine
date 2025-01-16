#include "pch.h"
#include "merlin/core/core.h"
#include "merlin/graphics/deferredRenderer.h"




namespace Merlin {

	DeferredRenderer::DeferredRenderer() {}

	DeferredRenderer::~DeferredRenderer() {}


	void DeferredRenderer::renderScene(const Scene& scene, const Camera& camera) {
		if (debug)Console::info() << "Rendering scene" << Console::endl;
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


	void DeferredRenderer::renderDepth(const Shared<RenderableObject>& object, Shared<Shader> shader){
		if (debug)Console::info() << "Rendering Depth" << Console::endl;

	}

	void DeferredRenderer::geometryPass(const Scene& scene, const Camera& camera){

	}

	void DeferredRenderer::lightingPass(const Camera& camera)
	{
	}

	void DeferredRenderer::finalCompositionPass()
	{
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
		render(obj.getXAxisMesh(), camera);
		render(obj.getYAxisMesh(), camera);
		render(obj.getZAxisMesh(), camera);
	}

	void DeferredRenderer::render(const Shared<RenderableObject>& object, const Camera& camera) {
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