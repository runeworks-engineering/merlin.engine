#include "pch.h"
#include "merlin/scene/scene.h"
#include "merlin/utils/modelLoader.h"

namespace Merlin {


	Scene::Scene(std::string name) : RenderableObject(name){
		m_rootNode = createShared<RenderableObject>(name);
		m_currentNode = m_rootNode;
	}

	const std::list<shared<RenderableObject>>& Scene::nodes() const {
		return m_rootNode->children();
	}
	
	void Scene::add(shared<ParticleSystem> ps) {
		m_currentNode->addChild(ps);
	}
	/*
	void Scene::add(shared<deprecated_GenericParticleSystem> ps) {
		m_currentNode->addChild(ps);
	}*/

	void Scene::clear() {
		m_rootNode = createShared<RenderableObject>(name());
		m_currentNode = m_rootNode;
	}

	void Scene::add(shared<Model> mdl) {
		m_currentNode->addChild(mdl);
	}

	void Scene::add(shared<IsoSurface> iso) {
		m_currentNode->addChild(iso);
	}

	void Scene::add(shared<Mesh> mesh) {
		m_currentNode->addChild(mesh);
	}

	void Scene::add(shared<Light> light) {
		m_currentNode->addChild(light);
	}

	void Scene::remove(shared<RenderableObject> obj) {
		m_currentNode->removeChild(obj);
	}


	bool Scene::hasEnvironment() const{
		return m_environment != nullptr;
	}

	void Scene::setEnvironment(shared<Environment> sky) {
		m_environment = sky;
	}

	shared<Environment> Scene::getEnvironment() const {
		return m_environment;
	}

	void Scene::add(shared<TransformObject> t) {
		m_currentNode->addChild(t);
	}

	
}
