#include "pch.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/utils/util.h"
#include "merlin/physics/samplers/voxelSampler.h"

using namespace Merlin::Utils;

namespace Merlin {

	PhysicsEntity::PhysicsEntity(const std::string& name, ParticleSampler_Ptr sampler) :
		m_name(name), 
		m_id(get_uuid()),
		m_sampler(sampler)
	{}

	const std::vector<glm::vec4>& PhysicsEntity::sample(bool forceReSample) {
		if (!hasSampler()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no sampler" << Console::endl;
			return std::vector<glm::vec4>();
		}

		if (!hasMesh()) {
			Console::error("PhysicsEntity(" + m_name + ")") << " Entity has no mesh" << Console::endl;
			return std::vector<glm::vec4>();
		}

		if (m_samples.size() == 0 || forceReSample) m_samples = m_sampler->sample(m_mesh);

		return m_samples;
	}

	bool PhysicsEntity::hasSampler() const {
		return m_sampler != nullptr;
	}

	void PhysicsEntity::addModifier(PhysicsModifier_Ptr phm) {
		if (phm) {
			if(hasModifier(phm->type())) Console::info("PhysicsEntity(" + m_name + ")") << "Modifier overwritten" << Console::endl;
			
			if ((phm->type() == PhysicsModifierType::FLUID ||
				phm->type() == PhysicsModifierType::SOFT_BODY || 
				phm->type() == PhysicsModifierType::RIGID_BODY || 
				phm->type() == PhysicsModifierType::GRANULAR_BODY) &&
				hasPhase()
				) {
				Console::error("PhysicsEntity") << "Entities can only have one phase" << Console::endl;
				return;
			}

			m_modifiers[phm->type()] = phm;
			if (phm->type() == PhysicsModifierType::EMITTER) {
				getModifier<Emitter>()->setPhase(getPhase());
			}
		}
	}

	const std::map<PhysicsModifierType, PhysicsModifier_Ptr>& PhysicsEntity::getModifiers() {
		return m_modifiers;
	}

	GLuint PhysicsEntity::getPhase() const{
		if (!hasPhase()) return 0;
		if (hasModifier(PhysicsModifierType::FLUID))
			return MaterialPhase::MATERIAL_FLUID;
		else if (hasModifier(PhysicsModifierType::SOFT_BODY))
			return MaterialPhase::MATERIAL_SOFT;
		else if (hasModifier(PhysicsModifierType::GRANULAR_BODY))
			return MaterialPhase::MATERIAL_GRANULAR;
		else if (hasModifier(PhysicsModifierType::RIGID_BODY)) 
			return MaterialPhase::MATERIAL_RIGID;

		//getModifier<PhaseChanger>();
		//if(hasModifier(PhysicsModifierType::PHASE_CHANGER))
			//return ;

		return 0;
	}

	glm::vec4 PhysicsEntity::getInitialVelocity() const
	{
		return glm::vec4(0);
	}

	float PhysicsEntity::getInitialTemperature() const
	{
		return 0.0f;
	}

	float PhysicsEntity::getInitialDensity() const
	{
		return 0.0f;
	}

	const std::string& PhysicsEntity::id()
	{
		return m_id;
	}

	const std::string& PhysicsEntity::name()
	{
		return m_name;
	}

	void PhysicsEntity::onRenderMenu()	{
		ImGui::Text((m_name + std::string(" properties")).c_str());

		ImGui::Text("ID: %s", m_id.c_str());
		ImGui::Text("Active: %s", m_active ? "true" : "false");
		ImGui::Text("Dynamic: %s", m_dynamic ? "true" : "false");
		ImGui::Text("Emitter: %s", m_emitter ? "true" : "false");
		if (hasMesh()) {
			if (ImGui::TreeNode("Mesh")) {
					
				ImGui::Text(m_mesh->name().c_str());
				//bounding box, etc
				ImGui::Text("Bounding Box");
				BoundingBox bb = m_mesh->getBoundingBox();
				ImGui::Text("Min: (%f, %f, %f)", bb.min.x, bb.min.y, bb.min.z);
				ImGui::Text("Max: (%f, %f, %f)", bb.max.x, bb.max.y, bb.max.z);
				ImGui::Text("Size: (%f, %f, %f)", bb.size.x, bb.size.y, bb.size.z);

				//particle count
				ImGui::Text("Particle Count: %d", m_samples.size());
				ImGui::TreePop();
			}
		}
		else {
			ImGui::Text("No mesh");
		}
		if (ImGui::TreeNode("Modifiers")) {
			for (const auto& mod : m_modifiers) {
				mod.second->onRenderMenu();
			}
			ImGui::TreePop();
		}

	}

	void PhysicsEntity::setSampler(ParticleSampler_Ptr smpl) {
		m_sampler = smpl;
	}

	void PhysicsEntity::setMesh(Mesh_Ptr mesh) {
		m_mesh = mesh;
	}

	BoundingBox PhysicsEntity::getBoundingBox() const {
		if (hasMesh()) return m_mesh->getBoundingBox();
		else {
			Console::error("PhysicsEntity(" + m_name + ")") << "Entity has no mesh" << Console::endl;
			return BoundingBox();
		}
	}

	bool PhysicsEntity::hasModifier(PhysicsModifierType phmt) const {
		return m_modifiers.find(phmt) != m_modifiers.end();
	}

	bool PhysicsEntity::hasMesh() const {
		return m_mesh != nullptr;
	}

	bool PhysicsEntity::hasPhase() const{
		return hasModifier(PhysicsModifierType::FLUID)			|| 
			   hasModifier(PhysicsModifierType::GRANULAR_BODY)  || 
			   hasModifier(PhysicsModifierType::RIGID_BODY)     || 
			   hasModifier(PhysicsModifierType::SOFT_BODY);
	}



	bool PhysicsEntity::isActive() const {
		return m_active;
	}
	
	void PhysicsEntity::isActive(bool state) {
		m_active = state;
	}

	bool PhysicsEntity::isEmitter() const {
		return hasModifier(PhysicsModifierType::EMITTER);
	}

}