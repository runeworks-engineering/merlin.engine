#include "pch.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/utils/util.h"

namespace Merlin {

	PhysicsEntity::PhysicsEntity(const std::string& name) : m_name(name), m_id(get_uuid()){
		
	}


	void PhysicsEntity::addModifier(PhysicsModifier_Ptr phm) {
		if (phm) {
			if(hasModifier(phm->type())) Console::info("PhysicsEntity(" + m_name + ")") << "Modifier overwritten" << Console::endl;
			m_modifiers[phm->type()] = phm;
		}
	}

	BoundingBox PhysicsEntity::getBoundingBox() const {
		if (hasMesh()) return m_mesh->getBoundingBox();
		else {
			Console::error("PhysicsEntity(" + m_name + ")") << "Entity has no mesh" << Console::endl;
			return BoundingBox();
		}
	}

	bool PhysicsEntity::hasModifier(PhysicsModifierType phmt) const {
		return m_modifiers.find(phmt) != m_modifiers.end();
	}

	bool PhysicsEntity::hasMesh() const {
		return m_mesh != nullptr;
	}

	bool PhysicsEntity::isActive() const {
		return m_active;
	}

	

}