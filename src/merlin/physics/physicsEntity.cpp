#include "pch.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/utils/util.h"
#include "merlin/physics/samplers/voxelSampler.h"




namespace Merlin {

	PhysicsEntity::PhysicsEntity(const std::string& name, ParticleSampler_Ptr sampler) :
		m_name(name), 
		m_id(get_uuid()),
		m_sampler(sampler)
	{}


	std::vector<glm::vec3> PhysicsEntity::sample() {
		if (!hasSampler()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no sampler" << Console::endl;
			return std::vector<glm::vec3>();
		}

		if (!hasMesh()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no mesh" << Console::endl;
			return std::vector<glm::vec3>();
		}

		return m_sampler->sample(m_mesh);
	}

	bool PhysicsEntity::hasSampler() const {
		return m_sampler != nullptr;
	}

	void PhysicsEntity::addModifier(PhysicsModifier_Ptr phm) {
		if (phm) {
			if(hasModifier(phm->type())) Console::info("PhysicsEntity(" + m_name + ")") << "Modifier overwritten" << Console::endl;
			m_modifiers[phm->type()] = phm;
		}
	}

	const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& PhysicsEntity::getModifiers() {
		return m_modifiers;
	}

	void PhysicsEntity::setSampler(ParticleSampler_Ptr smpl) {
		if (smpl) {
			m_sampler = smpl;
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