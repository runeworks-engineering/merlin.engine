#pragma once
#include "merlin/physics/particleSampler.h"

#include "merlin/physics/modifiers/fluid.h"
#include "merlin/physics/modifiers/rigidBody.h"
#include "merlin/physics/modifiers/softBody.h"
#include "merlin/physics/modifiers/granularBody.h"
#include "merlin/physics/modifiers/heatTransfer.h"
#include "merlin/physics/modifiers/phaseChanger.h"
#include "merlin/physics/modifiers/plasticity.h"
#include "merlin/physics/modifiers/emitter.h"

#include<map>

namespace Merlin{

	enum MaterialPhase {
		MATERIAL_UNUSED = 0,
		MATERIAL_FLUID = 1,
		MATERIAL_GRANULAR = 2,
		MATERIAL_RIGID = 2,
		MATERIAL_SOFT = 3,
		MATERIAL_BOUNDARY = 4
	};

	class PhysicsEntity{
	public:
		PhysicsEntity(const std::string& name, ParticleSampler_Ptr sampler);

		void addModifier(PhysicsModifier_Ptr);
		void setSampler(ParticleSampler_Ptr);

		std::vector<glm::vec4> sample();

		BoundingBox getBoundingBox() const;

		bool hasModifier(PhysicsModifierType) const;
		bool hasMesh() const;
		bool hasPhase() const;
		bool hasSampler() const;

		bool isActive() const;
		void isActive(bool state);

		bool isEmitter() const;
	
		const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& getModifiers();

		template<typename T>
		shared<T> getModifier();

		GLuint getPhase() const;

	private:
		bool m_active = true; //used to disable entity
		bool m_dynamic = true; //static otherwise
		bool m_emitter = false; //only for fluid, solid, rigid

		std::map<PhysicsModifierType,PhysicsModifier_Ptr> m_modifiers;
		std::string m_name;
		std::string m_id;
		Mesh_Ptr m_mesh = nullptr;
		ParticleSampler_Ptr m_sampler = nullptr;
	};

	typedef shared<PhysicsEntity> PhysicsEntity_Ptr;

}
/**/
template<>
inline shared<Fluid> PhysicsEntity::getModifier<Fluid>() {
	if (hasModifier(PhysicsModifierType::FLUID))
		return std::dynamic_pointer_cast<Fluid>(m_modifiers[PhysicsModifierType::FLUID]);
}

template<>
inline shared<SoftBody> PhysicsEntity::getModifier<SoftBody>() {
	if (hasModifier(PhysicsModifierType::SOFT_BODY))
		return std::dynamic_pointer_cast<SoftBody>(m_modifiers[PhysicsModifierType::SOFT_BODY]);
}

template<>
inline shared<RigidBody> PhysicsEntity::getModifier<RigidBody>() {
	if (hasModifier(PhysicsModifierType::RIGID_BODY))
		return std::dynamic_pointer_cast<RigidBody>(m_modifiers[PhysicsModifierType::RIGID_BODY]);
}

template<>
inline shared<GranularBody> PhysicsEntity::getModifier<GranularBody>() {
	if (hasModifier(PhysicsModifierType::GRANULAR_BODY))
		return std::dynamic_pointer_cast<GranularBody>(m_modifiers[PhysicsModifierType::GRANULAR_BODY]);
}

template<>
inline shared<HeatTransfer> PhysicsEntity::getModifier<HeatTransfer>() {
	if (hasModifier(PhysicsModifierType::HEAT_TRANSFER))
		return std::dynamic_pointer_cast<HeatTransfer>(m_modifiers[PhysicsModifierType::HEAT_TRANSFER]);
}

template<>
inline shared<PhaseChanger> PhysicsEntity::getModifier<PhaseChanger>() {
	if (hasModifier(PhysicsModifierType::PHASE_CHANGER))
		return std::dynamic_pointer_cast<PhaseChanger>(m_modifiers[PhysicsModifierType::PHASE_CHANGER]);
}

template<>
inline shared<Plasticity> PhysicsEntity::getModifier<Plasticity>() {
	if (hasModifier(PhysicsModifierType::PLASTICITY))
		return std::dynamic_pointer_cast<Plasticity>(m_modifiers[PhysicsModifierType::PLASTICITY]);
}

template<>
inline shared<Emitter> PhysicsEntity::getModifier<Emitter>() {
	if (hasModifier(PhysicsModifierType::EMITTER))
		return std::dynamic_pointer_cast<Emitter>(m_modifiers[PhysicsModifierType::EMITTER]);
}/**/