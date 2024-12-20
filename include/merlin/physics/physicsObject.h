#pragma once
#include "merlin/physics/physicsModifier.h"
#include<vector>

namespace Merlin{

	class PhysicsObject{
	public:




	private:
		bool dynamic = true;
		std::vector<PhysicsModifier> modifiers;

	};


}
