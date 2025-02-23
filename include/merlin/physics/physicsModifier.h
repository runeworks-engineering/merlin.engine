#pragma once
#include "merlin/core/core.h"


namespace Merlin {
	enum class PhysicsModifierType{
		FLUID,
		SOFT_BODY, 
		RIGID_BODY,
		HEAT_TRANSFER,
		PHASE_CHANGER
	};

	class PhysicsModifier{
	public:
		PhysicsModifier(PhysicsModifierType);
		~PhysicsModifier(){};

		void enable(bool state = true);
		void disable();

		PhysicsModifierType type();

	private:
		bool m_active = true;
		PhysicsModifierType m_type;
	};
	typedef shared<PhysicsModifier> PhysicsModifier_Ptr;
}

