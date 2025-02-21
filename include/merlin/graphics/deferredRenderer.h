#pragma once
#include "merlin/graphics/rendererBase.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/physics/isoSurface.h"
#include "merlin/memory/frameBuffer.h"
#include <stack>

namespace Merlin {

	class DeferredRenderer : public RendererBase {
	public:
		DeferredRenderer();
		~DeferredRenderer();

		//Render methods
		void renderScene(const Scene& scene, const Camera& camera);

	private:
		void render(const shared<RenderableObject>& object, const Camera& camera);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);
		void renderDepth(const shared<RenderableObject>& object, shared<Shader> depthShader);

		void geometryPass(const Scene& scene, const Camera& camera);
		void lightingPass(const Camera& camera);
		void finalCompositionPass();
		void applyPostProcessing();

		void setupGBuffer(int width, int height);
		void bindGBuffer();
		void unbindGBuffer();
	private:

		FBO_Ptr g_buffer; //Geometry pass
		FBO_Ptr g_buffer_final; //For post-process

		Shader_Ptr m_lightingShader;
		Shader_Ptr m_postProcessShader;

		// Post-processing and tone mapping
		bool m_enablePostProcessing = true;

	};

}
