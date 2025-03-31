#include "pch.h"
#include "merlin/physics/particleSystem.h"
#include "merlin/utils/primitives.h"
#include "merlin/utils/ressourceManager.h"

namespace Merlin{


	shared<ParticleSystem> ParticleSystem::create(const std::string& name, size_t count) {
		return std::make_shared<ParticleSystem>(name, count);
	}

	ParticleSystem::ParticleSystem(const std::string& name, size_t count) : RenderableObject(name), m_instancesCount(count), m_active_instancesCount(count) {
		m_geometry = Merlin::Primitives::createPoint();
	}

	void ParticleSystem::draw() const { 

		switch (m_displayMode) {
		case ParticleSystemDisplayMode::MESH:
			if(m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			break;
		case ParticleSystemDisplayMode::POINT_SPRITE :
			glEnable(GL_PROGRAM_POINT_SIZE);
			if (m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			break;
		case ParticleSystemDisplayMode::POINT_SPRITE_SHADED:
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(0x8861);//Point shading
			if (m_geometry) m_geometry->drawInstanced(m_active_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(0x8861);
			break;
		}
	} //draw the mesh

	void ParticleSystem::setInstancesCount(size_t count) {
		if (count == m_instancesCount) return;
		//if(m_fields.size() != 0) Console::warn() << "(Performance) Buffers of the particle system are being resized dynamically" << Console::endl;
		m_active_instancesCount = m_instancesCount = count;
		
		//for (auto field : m_fields) {
		//	field.second->resizeBuffer(count * field.second->type());
		//}
	}

	void ParticleSystem::setActiveInstancesCount(size_t count){
		if (count <= m_instancesCount)
			m_active_instancesCount = count;
		else {
			Console::error() << "Active instance count is greater than the total instance count" << Console::endl;
		}
		
	}



	bool ParticleSystem::hasPositionBuffer() const {
		if (m_position_buffer == nullptr) return false;
		return true;
	}

	void ParticleSystem::setPositionBuffer(AbstractBufferObject_Ptr buffer) {
		if(buffer)
			m_position_buffer = buffer;
	}

	AbstractBufferObject_Ptr ParticleSystem::getPositionBuffer() const{
		return m_position_buffer;
	}

	void ParticleSystem::setShader(Shader_Ptr shader) {
		m_shader = shader;
		//m_currentProgram = m_shader->name();
		//m_links[shader->name()] = std::set<std::string>();
	}

	bool ParticleSystem::hasShader() const{
		return m_shader != nullptr;
	}
	Shader_Ptr ParticleSystem::getShader() const{
		return m_shader;
	}



}