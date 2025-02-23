#include "merlin/physics/physicsModifier.h"

#include "merlin/physics/physicsEntity.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/particleSystem.h"

namespace Merlin {
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
	inline shared<HeatTransfer> PhysicsEntity::getModifier<HeatTransfer>() {
		if (hasModifier(PhysicsModifierType::HEAT_TRANSFER))
			return std::dynamic_pointer_cast<HeatTransfer>(m_modifiers[PhysicsModifierType::HEAT_TRANSFER]);
	}

	template<>
	inline shared<PhaseChanger> PhysicsEntity::getModifier<PhaseChanger>() {
		if (hasModifier(PhysicsModifierType::PHASE_CHANGER))
			return std::dynamic_pointer_cast<PhaseChanger>(m_modifiers[PhysicsModifierType::PHASE_CHANGER]);
	}
}