#pragma once
#include "merlin/core/core.h"


namespace Merlin {
	enum class PhysicsModifierType{
		FLUID,
		SOFT_BODY,
		RIGID_BODY,
		GRANULAR_BODY,
		HEAT_TRANSFER,
		PHASE_CHANGER,
		PLASTICITY,
		EMITTER
	};

	class PhysicsModifier{
	public:
		PhysicsModifier(PhysicsModifierType);
		~PhysicsModifier(){};

		void enable(bool state = true);
		void disable();

		bool isActive() const;
		PhysicsModifierType type();

		virtual void onRenderMenu() {};
		virtual void dummy() {};

		static std::string toString(PhysicsModifierType);

	private:
		bool m_active = true;
		PhysicsModifierType m_type;
	};

	typedef shared<PhysicsModifier> PhysicsModifier_Ptr;
}

