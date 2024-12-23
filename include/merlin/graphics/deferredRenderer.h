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
		void render(const Shared<RenderableObject>& object, const Camera& camera);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);
	
	public:

		// Light management
		void renderDepth(const Shared<RenderableObject>& object, Shared<Shader> depthShader);

		// G-buffer and lighting pass management
		void geometryPass(const Scene& scene, const Camera& camera);
		void lightingPass(const Camera& camera);
		void finalCompositionPass();

		// Environment and post-processing
		void renderEnvironment(const Environment& env, const Camera& camera);
		void applyPostProcessing();


	private:

		FBO_Ptr g_buffer;

		// G-buffer setup
		GLuint m_gBuffer;
		GLuint m_gPosition, m_gNormal, m_gAlbedoSpec, m_gMaterial;

		// Post-processing and tone mapping
		bool m_enablePostProcessing = true;

		// Utility functions
		void setupGBuffer(int width, int height);
		void bindGBuffer();
		void unbindGBuffer();
	};

}
