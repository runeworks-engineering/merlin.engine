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

	std::vector<glm::vec4> PhysicsEntity::sample() {
		if (!hasSampler()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no sampler" << Console::endl;
			return std::vector<glm::vec4>();
		}

		if (!hasMesh()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no mesh" << Console::endl;
			return std::vector<glm::vec4>();
		}

		return m_sampler->sample(m_mesh);
	}

	bool PhysicsEntity::hasSampler() const {
		return m_sampler != nullptr;
	}

	void PhysicsEntity::addModifier(PhysicsModifier_Ptr phm) {
		if (phm) {
			if(hasModifier(phm->type())) Console::info("PhysicsEntity(" + m_name + ")") << "Modifier overwritten" << Console::endl;
			
			if ((phm->type() == PhysicsModifierType::FLUID ||
				phm->type() == PhysicsModifierType::SOFT_BODY || 
				phm->type() == PhysicsModifierType::RIGID_BODY || 
				phm->type() == PhysicsModifierType::GRANULAR_BODY) &&
				hasPhase()
				) {
				Console::error("PhysicsEntity") << "Entities can only have one phase" << Console::endl;
				return;
			}

			m_modifiers[phm->type()] = phm;
			if (phm->type() == PhysicsModifierType::EMITTER) {
				getModifier<Emitter>()->setPhase(getPhase());
			}
		}
	}

	const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& PhysicsEntity::getModifiers() {
		return m_modifiers;
	}

	GLuint PhysicsEntity::getPhase() const{
		if (!hasPhase()) return 0;
		if (hasModifier(PhysicsModifierType::FLUID))
			return MaterialPhase::MATERIAL_FLUID;
		else if (hasModifier(PhysicsModifierType::SOFT_BODY))
			return MaterialPhase::MATERIAL_SOFT;
		else if (hasModifier(PhysicsModifierType::GRANULAR_BODY))
			return MaterialPhase::MATERIAL_GRANULAR;
		else if (hasModifier(PhysicsModifierType::RIGID_BODY)) 
			return MaterialPhase::MATERIAL_RIGID;

		return 0;
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

	bool PhysicsEntity::hasPhase() const{
		return hasModifier(PhysicsModifierType::FLUID)			|| 
			   hasModifier(PhysicsModifierType::GRANULAR_BODY)  || 
			   hasModifier(PhysicsModifierType::RIGID_BODY)     || 
			   hasModifier(PhysicsModifierType::SOFT_BODY);
	}



	bool PhysicsEntity::isActive() const {
		return m_active;
	}
	
	void PhysicsEntity::isActive(bool state) {
		m_active = state;
	}

	bool PhysicsEntity::isEmitter() const {
		return hasModifier(PhysicsModifierType::EMITTER);
	}

}