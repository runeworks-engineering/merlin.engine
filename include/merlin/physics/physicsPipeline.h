#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/ssbo.h"
#include "merlin/shaders/computeShader.h"
#include "merlin/physics/physicsEnum.h"

#include <string>
#include <vector>
#include <set>
#include <unordered_map>



namespace Merlin{


    class PhysicsPipelineStep {
    public:
		PhysicsPipelineStep(const std::string& name);

        void setProgram(ComputeShader_Ptr shader);
        void setProgram(StagedComputeShader_Ptr shader, int stage);
        void attachBuffer(BufferType type, const std::string& bufferName);
        const std::unordered_map<std::string, BufferType>& requiredBuffers() const;

        void setIteration(int i);
        void execute();
		virtual void onExecution();

		inline bool enabled() const { return m_enabled; }

        virtual const std::string& name() const = 0;

		//static shared<PhysicsPipelineStep> create(const std::string& name);


    protected:
		bool m_enabled = false;
        // One of these should be set.
        ComputeShader_Ptr       m_computeShader;
        StagedComputeShader_Ptr m_stagedComputeShader;
        int                     m_stage = 0;

        int m_iteration = 1;
        std::unordered_map<std::string, BufferType> m_requiredBuffers;
        std::vector<shared<PhysicsPipelineStep>> m_children;
    };

	typedef shared<PhysicsPipelineStep> PhysicsPipelineStep_Ptr;


	class PhysicsPipeline {
	public:
		PhysicsPipeline(const std::string& name);

        void addStep(std::shared_ptr<PhysicsPipelineStep> step);
        shared<PhysicsPipelineStep> getStep(int i) const;

        void initialize();
		void execute();

        // Accessors for internal maps (if necessary).
        std::unordered_map<std::string, AbstractBufferObject_Ptr>& buffers() { return m_buffers; }
        std::unordered_map<std::string, AbstractBufferObject_Ptr>& fields() { return m_fields; }
        std::unordered_map<std::string, ComputeShader_Ptr>& programs() { return m_programs; }

		//Memory 
		void addField(AbstractBufferObject_Ptr buf);
		bool hasField(const std::string& name) const;
		void clearField(const std::string& name);

		void addBuffer(AbstractBufferObject_Ptr buf);
		bool hasBuffer(const std::string& name) const;
		void clearBuffer(const std::string& name);
		
		void removeAllField();
		void removeAllBuffer();

		void writeField(const std::string& name, GLsizei typesize, void* data);
		void writeBuffer(const std::string& name, GLsizei typesize, GLsizei elements, void* data);

		template<typename T>
		void writeField(const std::string& name, std::vector<T> data);

		template<typename T>
		void writeBuffer(const std::string& name, std::vector<T> data);

		void link(const std::string& shader, const std::string& field);
		void detach(shared<ShaderBase>);
		void solveLink(shared<ShaderBase>);
		bool hasLink(const std::string& name) const;

		AbstractBufferObject_Ptr getField(const std::string& name) const;
		AbstractBufferObject_Ptr getBuffer(const std::string& name) const;

		//templates
		template<typename T>
		void addField(const std::string& name, bool sortable);

		template<typename T>
		void addBuffer(const std::string& name, GLsizei size = 0);

    protected:
		std::string m_name;

        // Pipeline steps stored by name.
        std::vector<PhysicsPipelineStep_Ptr> m_steps;

        // Containers for user-defined fields and buffers.
        std::unordered_map<std::string, std::set<std::string>> m_links;
        std::unordered_map<std::string, AbstractBufferObject_Ptr> m_fields;
        std::unordered_map<std::string, AbstractBufferObject_Ptr> m_buffers;

        // Container for compute shader programs.
        std::unordered_map<std::string, ComputeShader_Ptr> m_programs;

		GLsizeiptr m_default_fields_size = 0;
		GLuint m_default_workgroup_size = 1024; //Number of thread per workgroup (particles)

    };

    typedef shared<PhysicsPipeline> PhysicsPipeline_Ptr;


	template<typename T>
	void PhysicsPipeline::addField(const std::string& name, bool sortable) {
		if (hasField(name)) {
			Console::warn("PhysicsPipeline") << name << "has been overwritten" << Console::endl;
		}
		SSBO_Ptr<T> f = SSBO<T>::create(name, m_default_fields_size);
		m_fields[name] = f;

		/*
		if (sortable) {
			m_sortableFields[name] = sizeof(T);
		}*/

		/*
		for(const auto& prgm : m_programs)
			if (hasLink(prgm.first)) {
				link(prgm.first, f->name());
			}*/
	}

	template<typename T>
	void PhysicsPipeline::addBuffer(const std::string& name, GLsizei size) {
		if (hasBuffer(name)) {
			Console::warn("PhysicsPipeline") << name << "has been overwritten" << Console::endl;
		}
		SSBO_Ptr<T> f = SSBO<T>::create(name, size);
		m_buffers[name] = f;
		/*
		for (const auto& prgm : m_programs)
			if (hasLink(prgm.first)) {
				link(prgm.first, f->name());
			}*/
	}

	template<typename T>
	void PhysicsPipeline::writeField(const std::string& name, std::vector<T> data) {
		if (hasField(name)) {
			if (m_fields[name]->elements() < data.size()) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_fields[name]->allocateBuffer(data.size() * sizeof(T), data.data(), BufferUsage::StaticDraw);
			}
			else m_fields[name]->writeBuffer(data.size() * sizeof(T), data.data());
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the physics pipeline." << Console::endl;
	}

	template<typename T>
	void PhysicsPipeline::writeBuffer(const std::string& name, std::vector<T> data) {
		if (hasBuffer(name)) {
			if (m_buffers[name]->elements() < data.size()) {
				//Console::error("ParticleSystem") << "Field hasn't been allocated" << Console::endl;
				m_buffers[name]->allocateBuffer(data.size() * sizeof(T), data.data(), BufferUsage::StaticDraw);
			}
			else m_buffers[name]->writeBuffer(data.size() * sizeof(T), data.data());
		}
		else Console::error("PhysicsPipeline") << name << " is not registered in the physics pipeline." << Console::endl;
	}





}