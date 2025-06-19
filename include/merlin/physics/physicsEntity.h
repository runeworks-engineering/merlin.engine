#pragma once
#include "merlin/physics/physicsEnum.h"
#include "merlin/physics/particleSampler.h"

#include "merlin/physics/modifiers/fluid.h"
#include "merlin/physics/modifiers/rigidBody.h"
#include "merlin/physics/modifiers/softBody.h"
#include "merlin/physics/modifiers/granularBody.h"
#include "merlin/physics/modifiers/heatTransfer.h"
#include "merlin/physics/modifiers/phaseChanger.h"
#include "merlin/physics/modifiers/plasticity.h"
#include "merlin/physics/modifiers/emitter.h"

#include<map>

namespace Merlin{

	class PhysicsEntity{
	public:
		PhysicsEntity(const std::string& name, ParticleSampler_Ptr sampler);

		void addModifier(PhysicsModifier_Ptr);
		void setSampler(ParticleSampler_Ptr);
		void setMesh(Mesh_Ptr mesh);

		template<typename T>
		void addInitialValue(const std::string& name, BufferType, T* value);
		bool hasInitialValue(const std::string& name);

		const std::vector<glm::vec4>& sample(bool = false);

		BoundingBox getBoundingBox() const;

		bool hasModifier(PhysicsModifierType) const;
		bool hasMesh() const;
		bool hasPhase() const;
		bool hasSampler() const;

		bool isActive() const;
		void isActive(bool state);
		bool isEmitter() const;
	
		const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& getModifiers();

		template<typename T>
		shared<T> getModifier();

		//TODO implements the following givie the modifiers
		GLuint getPhase() const;
		glm::vec4 getInitialVelocity() const;
		float getInitialTemperature() const;
		float getInitialDensity() const;

		const std::string& id();
		const std::string& name();

		void onRenderMenu();

	private:
		bool m_active = true; //used to disable entity
		bool m_dynamic = true; //static otherwise
		bool m_emitter = false; //only for fluid, solid, rigid

		std::vector<glm::vec4> m_samples;
		std::unordered_map<std::string, std::pair<BufferType, void*>> m_init_values;

		ColorMap m_currentMap;
		PhysicsField m_displayField;

		std::map<PhysicsModifierType,PhysicsModifier_Ptr> m_modifiers;
		std::string m_name;
		std::string m_id;
		Mesh_Ptr m_mesh = nullptr;
		ParticleSampler_Ptr m_sampler = nullptr;
	};

	typedef shared<PhysicsEntity> PhysicsEntity_Ptr;

	template<typename T>
	inline void PhysicsEntity::addInitialValue(const std::string& name, BufferType type, T* value){
		m_init_values[name] = std::make_pair(type, (void*)value);
	}

	template<>
	inline shared<FluidModifier> PhysicsEntity::getModifier<FluidModifier>() {
		if (hasModifier(PhysicsModifierType::FLUID))
			return std::dynamic_pointer_cast<FluidModifier>(m_modifiers[PhysicsModifierType::FLUID]);
		return nullptr;
	}

	template<>
	inline shared<SoftBody> PhysicsEntity::getModifier<SoftBody>() {
		if (hasModifier(PhysicsModifierType::SOFT_BODY))
			return std::dynamic_pointer_cast<SoftBody>(m_modifiers[PhysicsModifierType::SOFT_BODY]);
		return nullptr;
	}

	template<>
	inline shared<RigidBody> PhysicsEntity::getModifier<RigidBody>() {
		if (hasModifier(PhysicsModifierType::RIGID_BODY))
			return std::dynamic_pointer_cast<RigidBody>(m_modifiers[PhysicsModifierType::RIGID_BODY]);
		return nullptr;
	}

	template<>
	inline shared<GranularBody> PhysicsEntity::getModifier<GranularBody>() {
		if (hasModifier(PhysicsModifierType::GRANULAR_BODY))
			return std::dynamic_pointer_cast<GranularBody>(m_modifiers[PhysicsModifierType::GRANULAR_BODY]);
		return nullptr;
	}

	template<>
	inline shared<HeatTransfer> PhysicsEntity::getModifier<HeatTransfer>() {
		if (hasModifier(PhysicsModifierType::HEAT_TRANSFER))
			return std::dynamic_pointer_cast<HeatTransfer>(m_modifiers[PhysicsModifierType::HEAT_TRANSFER]);
		return nullptr;
	}

	template<>
	inline shared<PhaseChanger> PhysicsEntity::getModifier<PhaseChanger>() {
		if (hasModifier(PhysicsModifierType::PHASE_CHANGER))
			return std::dynamic_pointer_cast<PhaseChanger>(m_modifiers[PhysicsModifierType::PHASE_CHANGER]);
		return nullptr;
	}

	template<>
	inline shared<Plasticity> PhysicsEntity::getModifier<Plasticity>() {
		if (hasModifier(PhysicsModifierType::PLASTICITY))
			return std::dynamic_pointer_cast<Plasticity>(m_modifiers[PhysicsModifierType::PLASTICITY]);
		return nullptr;
	}

	template<>
	inline shared<Emitter> PhysicsEntity::getModifier<Emitter>() {
		if (hasModifier(PhysicsModifierType::EMITTER))
			return std::dynamic_pointer_cast<Emitter>(m_modifiers[PhysicsModifierType::EMITTER]);
		return nullptr;
	}/**/
}