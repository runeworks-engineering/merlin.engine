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
		Renderer();
		~Renderer();

		//Render methods
		void render(const Shared<RenderableObject>& object, const Camera& camera);
		void renderScene(const Scene& model, const Camera& camera);
		void renderLight(const Light& li, const Camera& camera);
		void renderMesh(const Mesh& mesh, const Camera& camera);
		void renderEnvironment(const Environment& env, const Camera& camera);
		void renderParticleSystem(const ParticleSystem& ps, const Camera& camera);
		void renderTransformObject(const TransformObject& obj, const Camera& camera);

		//Shadows
		void castShadow(Shared<Light> light, const Scene& scene);
		void renderDepth(const Shared<RenderableObject>& object, Shared<Shader> shader);
		void gatherLights(const Shared<RenderableObject>& object);

	};
}
