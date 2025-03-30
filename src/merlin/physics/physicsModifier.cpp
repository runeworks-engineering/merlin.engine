#include "pch.h"
#include "merlin/physics/physicsModifier.h"

namespace Merlin {

	PhysicsModifier::PhysicsModifier(PhysicsModifierType type) : m_type(type) {
	};

	void PhysicsModifier::enable(bool state) {
		m_active = state;
	}

	void PhysicsModifier::disable() {
		m_active = false;
	}

	bool PhysicsModifier::isActive() const{
		return m_active;
	}

	PhysicsModifierType PhysicsModifier::type() {
		return m_type;
	}

	std::string PhysicsModifier::toString(PhysicsModifierType modifier)
	{
		switch (modifier)		{
		case Merlin::PhysicsModifierType::FLUID:
			return "Fluid";
			break;
		case Merlin::PhysicsModifierType::SOFT_BODY:
			return "Soft body";
			break;
		case Merlin::PhysicsModifierType::RIGID_BODY:
			return "Rigid body";
			break;
		case Merlin::PhysicsModifierType::GRANULAR_BODY:
			return "Granular body";
			break;
		case Merlin::PhysicsModifierType::HEAT_TRANSFER:
			return "Heat transfer";
			break;
		case Merlin::PhysicsModifierType::PHASE_CHANGER:
			return "Phase change";
			break;
		case Merlin::PhysicsModifierType::PLASTICITY:
			return "Plasticity";
			break;
		case Merlin::PhysicsModifierType::EMITTER:
			return "Emitter";
			break;
		default:
			break;
		}
	}

}