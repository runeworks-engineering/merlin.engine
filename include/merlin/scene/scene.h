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
		shared<RenderableObject> m_rootNode = nullptr;
		shared<RenderableObject> m_currentNode = nullptr;
		shared<Environment> m_environment = nullptr;

	public:
		Scene(std::string name = "scene");

		void add(shared<Mesh>);
		void add(shared<Model>);
		void add(shared<ParticleSystem>);
		void add(shared<IsoSurface>);
		void remove(shared<RenderableObject>);

		void clear();
		//void add(shared<deprecated_GenericParticleSystem>);
		void add(shared<Light>);
		void add(shared<TransformObject> t);

		bool hasEnvironment() const;
		void setEnvironment(shared<Environment> environment);
		shared<Environment> getEnvironment() const;

		const std::list<shared<RenderableObject>>& nodes() const;

		inline static shared<Scene> create() { return createShared<Scene>(); };
		inline static shared<Scene> create(std::string name) { return createShared<Scene>(name); };
	};
	
	typedef shared<Scene> Scene_Ptr;
}