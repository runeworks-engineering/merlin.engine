#include "pch.h"
#include "merlin/physics/physicsPipeline.h"
#include "merlin/utils/util.h"

namespace Merlin {

	AbstractBufferObject_Ptr PhysicsPipeline::getField(const std::string& name) const {
		if (hasField(name)) {
			return m_fields.at(name);
		}
		else {
			Console::error("PhysicsPipeline") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}

	AbstractBufferObject_Ptr PhysicsPipeline::getBuffer(const std::string& name) const {
		if (hasBuffer(name)) {
			return m_buffers.at(name);
		}
		else {
			Console::error("PhysicsPipeline") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}


	PhysicsPipeline::PhysicsPipeline(const std::string& name) : m_name(name)	{

	}

	void PhysicsPipeline::addStep(std::shared_ptr<PhysicsPipelineStep> step){
		m_steps.push_back(step);
	}

	void PhysicsPipeline::initialize(){

		//Generate buffers
		for (auto& step : m_steps) {
			const auto& reqbuffers = step->requiredBuffers();
			for (auto& buf : reqbuffers) {
				if (!hasField(buf.first)) {
					
				}
			}
		}


	}

	void PhysicsPipeline::execute()	{

	}

	shared<PhysicsPipelineStep> PhysicsPipeline::getStep(int step) const{
		if (step < 0 || step > m_steps.size()) return nullptr;
		return m_steps.at(step);
	}

	void PhysicsPipeline::addField(AbstractBufferObject_Ptr field) {
		if (hasField(field->name())) {
			Console::warn("PhysicsPipeline") << field->name() << "has been overwritten" << Console::endl;
		}
		m_fields[field->name()] = field;
	}

	void PhysicsPipeline::addBuffer(AbstractBufferObject_Ptr buf) {
		if (hasBuffer(buf->name())) {
			Console::warn("PhysicsPipeline") << buf->name() << "has been overwritten" << Console::endl;
		}
		m_buffers[buf->name()] = buf;
	}

	bool PhysicsPipeline::hasField(const std::string& name) const {
		return m_fields.find(name) != m_fields.end();
	}

	bool PhysicsPipeline::hasBuffer(const std::string& name) const {
		return m_buffers.find(name) != m_buffers.end();
	}

	void PhysicsPipeline::clearField(const std::string& name) {
		if (hasField(name)) {
			m_fields[name]->clearBuffer();
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the particle system." << Console::endl;
	}

	void PhysicsPipeline::clearBuffer(const std::string& name) {
		if (hasBuffer(name)) {
			m_buffers[name]->clearBuffer();
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the particle system." << Console::endl;
	}

	void PhysicsPipeline::removeAllField() {

		for (const auto& f : m_fields) {
			m_links.erase(f.first);
		}
		m_fields.clear();
	}

	void PhysicsPipeline::removeAllBuffer() {
		for (const auto& f : m_buffers) {
			m_links.erase(f.first);
		}
		m_buffers.clear();
	}


	void PhysicsPipeline::link(const std::string& shader, const std::string& field) {
		if (!hasLink(shader)) {
			m_links[shader] = std::set<std::string>();
		}
		m_links[shader].insert(field);
	}

	bool PhysicsPipeline::hasLink(const std::string& shader) const {
		return m_links.find(shader) != m_links.end();
	}

	void PhysicsPipeline::detach(shared<ShaderBase> shader) {
		if (hasLink(shader->name())) {
			for (auto& entry : m_links[shader->name()]) {
				if (hasField(entry))
					shader->detach(*getField(entry));
				else if (hasBuffer(entry))
					shader->detach(*getBuffer(entry));
				else
					Console::error("PhysicsPipeline") << entry << " is not registered as field in the pipeline. cannot detach" << Console::endl;
			}
		}
		else {
			Console::error("PhysicsPipeline") << shader->name() << " is not registered in the pipeline. cannot detach" << Console::endl;
		}
	}

	void PhysicsPipeline::solveLink(shared<ShaderBase> shader) {
		if (hasLink(shader->name())) {
			for (const auto& entry : m_links[shader->name()]) {
				if (hasField(entry))
					shader->attach(*getField(entry));
				else if (hasBuffer(entry))
					shader->attach(*getBuffer(entry));
				else
					Console::error("PhysicsPipeline") << entry << " is not registered as field in the pipeline. cannot attach" << Console::endl;

			}
		}
		else {
			Console::error("PhysicsPipeline") << shader->name() << " is not registered in the pipeline. cannot attach" << Console::endl;
		}
	}

	void PhysicsPipeline::writeField(const std::string& name, GLsizei typesize, void* data) {
		if (hasField(name)) {
			if (m_fields[name]->elements() < m_default_fields_size) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_fields[name]->allocateBuffer(m_default_fields_size * typesize, data, BufferUsage::StaticDraw);
			}
			else m_fields[name]->writeBuffer(m_default_fields_size * typesize, data);
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the pipeline." << Console::endl;
	}

	void PhysicsPipeline::writeBuffer(const std::string& name, GLsizei typesize, GLsizei elements, void* data) {
		if (hasBuffer(name)) {
			if (m_buffers[name]->elements() < elements) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_buffers[name]->allocateBuffer(elements * typesize, data, BufferUsage::StaticDraw);
			}
			else m_buffers[name]->writeBuffer(elements * typesize, data);
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the pipeline." << Console::endl;
	}




	/******************************************/
	/*****       PhysicsPipelineStep      *****/
	/******************************************/



	PhysicsPipelineStep::PhysicsPipelineStep(const std::string& name){

	}

	void PhysicsPipelineStep::setProgram(ComputeShader_Ptr shader)	{
		m_computeShader = shader;
		m_stagedComputeShader.reset();
	}

	void PhysicsPipelineStep::setProgram(StagedComputeShader_Ptr shader, int stage) {
		m_stagedComputeShader = shader;
		m_stage = stage;
		m_computeShader.reset();
	}

	void PhysicsPipelineStep::attachBuffer(BufferType type, const std::string& bufferName){
		m_requiredBuffers[bufferName] = type;
	}


	// Retrieve the list of required buffer names.
	const std::unordered_map<std::string, BufferType>& PhysicsPipelineStep::requiredBuffers() const {
		return m_requiredBuffers;
	}

	void PhysicsPipelineStep::setIteration(int i){
		m_iteration = i;
	}

	void PhysicsPipelineStep::execute() {
		for (int i = 0; i < m_iteration; i++) {
			if (m_computeShader) {
				m_computeShader->dispatch();
			}
			else if (m_stagedComputeShader) {
				m_stagedComputeShader->execute(m_stage);
			}

			for (const auto& child : m_children) {
				child->execute();
			}
		}
	}

	void PhysicsPipelineStep::onExecution() {
		if (m_computeShader) {
			m_computeShader->dispatch();
		}
		else if (m_stagedComputeShader) {
			m_stagedComputeShader->execute(m_stage);
		}
	}

	/*
	void PhysicsPipelineStep::setDispatchCount(int count) {
		m_dispatchCount = count;
	}

	int PhysicsPipelineStep::dispatchCount() const { return m_dispatchCount; }
	*/


}