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
