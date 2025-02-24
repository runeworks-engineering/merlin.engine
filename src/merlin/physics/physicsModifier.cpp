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

}