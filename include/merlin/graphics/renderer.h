#pragma once
#include "merlin/graphics/rendererBase.h"
#include "merlin/scene/scene.h"
#include "merlin/scene/camera.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/physics/isoSurface.h"
#include "merlin/memory/frameBuffer.h"

namespace Merlin {
	class Renderer : public RendererBase {
	public:
		Renderer() {};

		//Render methods
		void render(const shared<RenderableObject>& object, const Camera& camera);
		void renderScene(const Scene& model, const Camera& camera);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);

		//Shadows
		void castShadow(shared<Light> light, const Scene& scene);
		void renderDepth(const shared<RenderableObject>& object, shared<Shader> shader);
		void gatherLights(const shared<RenderableObject>& object);

	private:
		bool transparent_pass = false;
	};
}
