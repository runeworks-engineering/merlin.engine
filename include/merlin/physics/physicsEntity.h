#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/particleSampler.h"
#include "merlin/graphics/mesh.h"
#include<map>

#include "merlin/physics/modifiers/fluid.h"
#include "merlin/physics/modifiers/rigidBody.h"
#include "merlin/physics/modifiers/softBody.h"
#include "merlin/physics/modifiers/heatTransfer.h"
#include "merlin/physics/modifiers/phaseChanger.h"


namespace Merlin{

	class PhysicsEntity{
	public:
		PhysicsEntity(const std::string& name, ParticleSampler_Ptr sampler);

		void addModifier(PhysicsModifier_Ptr);
		void setSampler(ParticleSampler_Ptr);

		std::vector<glm::vec3> sample();

		BoundingBox getBoundingBox() const;

		bool hasModifier(PhysicsModifierType) const;
		bool hasMesh() const;
		bool isActive() const;
		bool hasSampler() const;

	
		const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& getModifiers();

		template<typename T>
		shared<T> getModifier();

	private:
		bool m_active = true; //used to disable entity
		bool m_dynamic = true; //static otherwise

		std::map<PhysicsModifierType,PhysicsModifier_Ptr> m_modifiers;
		std::string m_name;
		std::string m_id;
		Mesh_Ptr m_mesh = nullptr;
		ParticleSampler_Ptr m_sampler = nullptr;
	};

	typedef shared<PhysicsEntity> PhysicsEntity_Ptr;

}

template<>
shared<Fluid> PhysicsEntity::getModifier<Fluid>() {
	if (hasModifier(PhysicsModifierType::FLUID))
		return std::dynamic_pointer_cast<Fluid>(m_modifiers[PhysicsModifierType::FLUID]);
}

template<>
shared<SoftBody> PhysicsEntity::getModifier<SoftBody>() {
	if (hasModifier(PhysicsModifierType::SOFT_BODY))
		return std::dynamic_pointer_cast<SoftBody>(m_modifiers[PhysicsModifierType::SOFT_BODY]);
}

template<>
shared<RigidBody> PhysicsEntity::getModifier<RigidBody>() {
	if (hasModifier(PhysicsModifierType::RIGID_BODY))
		return std::dynamic_pointer_cast<RigidBody>(m_modifiers[PhysicsModifierType::RIGID_BODY]);
}

template<>
shared<HeatTransfer> PhysicsEntity::getModifier<HeatTransfer>() {
	if (hasModifier(PhysicsModifierType::HEAT_TRANSFER))
		return std::dynamic_pointer_cast<HeatTransfer>(m_modifiers[PhysicsModifierType::HEAT_TRANSFER]);
}

template<>
shared<PhaseChanger> PhysicsEntity::getModifier<PhaseChanger>() {
	if (hasModifier(PhysicsModifierType::PHASE_CHANGER))
		return std::dynamic_pointer_cast<PhaseChanger>(m_modifiers[PhysicsModifierType::PHASE_CHANGER]);
}

template<>
shared<ShellSampling> PhysicsEntity::getModifier<ShellSampling>() {
	if (hasModifier(PhysicsModifierType::SHELL_SAMPLING))
		return std::dynamic_pointer_cast<ShellSampling>(m_modifiers[PhysicsModifierType::SHELL_SAMPLING]);
}