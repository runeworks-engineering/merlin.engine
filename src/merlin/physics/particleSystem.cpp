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
		if(m_fields.size() != 0) Console::warn() << "(Performance) Buffers of the particle system are being resized dynamically" << Console::endl;
		m_active_instancesCount = m_instancesCount = count;
		
		for (auto field : m_fields) {
			field.second->resizeBuffer(count * field.second->type());
		}
	}

	void ParticleSystem::setActiveInstancesCount(size_t count){
		if (count <= m_instancesCount)
			m_active_instancesCount = count;
		else {
			Console::error() << "Active instance count is greater than the total instance count" << Console::endl;
		}
		
	}


	AbstractBufferObject_Ptr ParticleSystem::getField(const std::string& name) const {
		if (hasField(name)) {
			return m_fields.at(name);
		}
		else {
			Console::error("ParticleSystem") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}

	const std::map<std::string, GLuint>& ParticleSystem::getCopyBufferStructure() const{
		return m_sortableFields;
	}

	AbstractBufferObject_Ptr ParticleSystem::getBuffer(const std::string& name) const {
		if (hasBuffer(name)) {
			return m_buffers.at(name);
		}
		else {
			Console::error("ParticleSystem") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}


	void ParticleSystem::addField(AbstractBufferObject_Ptr field, bool sortable) {
		if (hasField(field->name())) {
			Console::warn("ParticleSystem") << field->name() << "has been overwritten" << Console::endl;
		}
		m_fields[field->name()] = field;

		if (sortable) {
			m_sortableFields[field->name()] = field->type(); //store the field data type size in fields list
		}

		if (hasLink(m_currentProgram)) {
			link(m_currentProgram, field->name());
		}
	}

	void ParticleSystem::addBuffer(AbstractBufferObject_Ptr buf){
		if (hasBuffer(buf->name())) {
			Console::warn("ParticleSystem") << buf->name() << "has been overwritten" << Console::endl;
		}
		m_buffers[buf->name()] = buf;
		if (hasLink(m_currentProgram)) {
			link(m_currentProgram, buf->name());
		}
	}

	bool ParticleSystem::hasField(const std::string& name) const{
		return m_fields.find(name) != m_fields.end();
	}

	bool ParticleSystem::hasBuffer(const std::string& name) const {
		return m_buffers.find(name) != m_buffers.end();
	}

	void ParticleSystem::clearField(const std::string& name) {
		if (hasField(name)) {
			m_fields[name]->clearBuffer();
		}
		else Console::error("ParticleSystem") << name << " is not registered in the particle system." << Console::endl;
	}

	void ParticleSystem::clearBuffer(const std::string& name) {
		if (hasBuffer(name)) {
			m_buffers[name]->clearBuffer();
		}
		else Console::error("ParticleSystem") << name << " is not registered in the particle system." << Console::endl;
	}

	void ParticleSystem::removeAllField() {

		for (const auto& f : m_fields) {
			m_links.erase(f.first);
		}
		m_fields.clear();
		m_sortableFields.clear();
	}

	void ParticleSystem::removeAllBuffer() {
		for (const auto& f : m_buffers) {
			m_links.erase(f.first);
		}
		m_buffers.clear();
	}



	void ParticleSystem::writeField(const std::string& name, GLsizei typesize,  void* data){
		if (hasField(name)) {
			if (m_fields[name]->elements() < m_instancesCount) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_fields[name]->allocateBuffer(m_instancesCount * typesize, data, BufferUsage::StaticDraw);
			} else m_fields[name]->writeBuffer(m_instancesCount * typesize, data);
		}else Console::error("ParticleSystem") << name << " is not registered in the particle system." << Console::endl;
	}

	void ParticleSystem::writeBuffer(const std::string& name, GLsizei typesize, GLsizei elements, void* data) {
		if (hasBuffer(name)) {
			if (m_buffers[name]->elements() < elements) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_buffers[name]->allocateBuffer(elements * typesize, data, BufferUsage::StaticDraw);
			}
			else m_buffers[name]->writeBuffer(elements * typesize, data);
		}else Console::error("ParticleSystem") << name << " is not registered in the particle system." << Console::endl;
	}

	void ParticleSystem::setShader(Shader_Ptr shader) {
		m_shader = shader;
		m_currentProgram = m_shader->name();
		m_links[shader->name()] = std::set<std::string>();
	}

	bool ParticleSystem::hasShader() const{
		return m_shader != nullptr;
	}
	Shader_Ptr ParticleSystem::getShader() const{
		return m_shader;
	}

	void ParticleSystem::link(const std::string& shader, const std::string& field) {
		if (!hasLink(shader)) {
			m_links[shader] = std::set<std::string>();
		}
		m_links[shader].insert(field);
	}

	bool ParticleSystem::hasLink(const std::string& shader) const{
		return m_links.find(shader) != m_links.end();
	}

	void ParticleSystem::detach(shared<ShaderBase> shader) {
		if (hasLink(shader->name())) {
			for (auto& entry : m_links[shader->name()]) {
				if (hasField(entry))
					shader->detach(*getField(entry));
				else if (hasBuffer(entry))
					shader->detach(*getBuffer(entry));
				else
					Console::error("ParticleSystem") << entry << " is not registered as field in the particle system. cannot detach" << Console::endl;
			}
		}
		else {
			Console::error("ParticleSystem") << shader->name() << " is not registered in the particle system. cannot detach" << Console::endl;
		}
	}

	void ParticleSystem::solveLink(shared<ShaderBase> shader) {
		if (hasLink(shader->name())) {
			for (auto& entry : m_links[shader->name()]) {
				if(hasField(entry))
					shader->attach(*getField(entry));
				else if(hasBuffer(entry))
					shader->attach(*getBuffer(entry));
				else
					Console::error("ParticleSystem") << entry << " is not registered as field in the particle system. cannot attach" << Console::endl;
				
			}
		}
		else {
			Console::error("ParticleSystem") << shader->name() << " is not registered in the particle system. cannot attach" << Console::endl;
		}
	}


}