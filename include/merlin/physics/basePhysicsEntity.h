#pragma once
#include "merlin/scene/model.h"

namespace Merlin {
	class BasePhysicsEntity{
	public:
		BasePhysicsEntity(){}

		virtual void stepPhysics(float dt);
		

	private:
		Model_Ptr model = nullptr;

	};
}
