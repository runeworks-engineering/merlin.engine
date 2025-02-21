#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/graphics/mesh.h"
#include<map>

namespace Merlin{

	class PhysicsEntity{
	public:
		PhysicsEntity(const std::string& name);
		void addModifier(PhysicsModifier_Ptr);

		BoundingBox getBoundingBox() const;

		bool hasModifier(PhysicsModifierType) const;
		bool hasMesh() const;
		bool isActive() const;


	private:
		bool m_active = true; //used to disable entity
		bool m_dynamic = true; //static otherwise

		std::map<PhysicsModifierType,PhysicsModifier_Ptr> m_modifiers;
		std::string m_name;
		std::string m_id;
		Mesh_Ptr m_mesh = nullptr;
	};

	typedef shared<PhysicsEntity> PhysicsEntity_Ptr;

}
