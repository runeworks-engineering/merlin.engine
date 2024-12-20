#pragma once
#include <vector>

#include "merlin/graphics/renderableObject.h"
#include "merlin/scene/model.h"
#include "merlin/scene/environment.h"
#include "merlin/scene/light.h"
#include "merlin/scene/transformObject.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/physics/isoSurface.h"
#include "camera.h"

namespace Merlin {

	class Scene : public RenderableObject {
		Shared<RenderableObject> m_rootNode = nullptr;
		Shared<RenderableObject> m_currentNode = nullptr;
		Shared<Environment> m_environment = nullptr;

	public:
		Scene(std::string name = "scene");

		void add(Shared<Mesh>);
		void add(Shared<Model>);
		void add(Shared<ParticleSystem>);
		void add(Shared<IsoSurface>);
		//void add(Shared<deprecated_GenericParticleSystem>);
		void add(Shared<Light>);
		void add(Shared<TransformObject> t);

		bool hasEnvironment() const;
		void setEnvironment(Shared<Environment> environment);
		Shared<Environment> getEnvironment() const;

		const std::list<Shared<RenderableObject>>& nodes() const;

		inline static Shared<Scene> create() { return createShared<Scene>(); };
	};
	
}