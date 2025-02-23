#include "pch.h"
#include "merlin/physics/physicsSolver.h"
#include "merlin/physics/particleSampler.h"

namespace Merlin {

    PhysicsSolver::PhysicsSolver() {

    }

    void PhysicsSolver::init() {
        // Log the start of the physics engine initialization.
        Console::info("PhysicsSolver") << "Physics engine starting..." << Console::endl;
        std::vector<std::vector<glm::vec3>> sampledParticles;

        // Iterate over each active entity and perform voxelized sampling.
        for (const auto& entity : m_entity) {
            if (entity->isActive()) {
               sampledParticles.push_back(entity->sample());
      
                // For each modifier attached to the entity, initialize the sample.
                // For example, the Heat Transfer modifier might assign an initial temperature.
                
                for (auto& modPair : entity->getModifiers()) {
                    //modPair.second->initializeSample(samples);
                }

                // Append this entity's samples to the overall particle sample vector.
               /////////  sampledParticles.insert(sampledParticles.end(), samples.begin(), samples.end());
            }
        }

        // Optionally pad the particle sample so that the particle system has a fixed size.
        /////////  while (sampledParticles.size() < particles->getMaxParticleCount()) {
        /////////     sampledParticles.push_back(ParticleData()); // Default-initialized particle.
        ///////// }

        // -------------------------------------------------------------------------
        // 4. Upload Initial Data and Link Buffers
        // -------------------------------------------------------------------------
        // Upload the sampled particle data to the GPU via the particle system.
        ///////// particles->initializeBuffers(sampledParticles);

        // Automatically bind the Shader Storage Buffer Objects (SSBOs) for both the
        // simulation and the bin systems to their respective compute shaders.
        ///////// particles->solveLink(/* pointer to solver shader, if needed */);
        ///////// bins->solveLink(/* pointer to bin shader, if needed */);

        m_particles = ParticleSystem::create("ParticleSystem", 1);
        m_bins = ParticleSystem::create("BinSystem", 1);

        // -------------------------------------------------------------------------
        // 5. Final Setup
        // -------------------------------------------------------------------------
        // Additional dynamic shader setup can be performed here.
        // For example, setting uniforms for the simulation domain:
        // solverShader->setVec3("domainMin", domain.getMin());
        // solverShader->setVec3("domainMax", domain.getMax());

        // Mark the physics engine as ready.
        Console::success("PhysicsSolver") << "Physics engine ready." << Console::endl;
        m_ready = true;
    }

	void PhysicsSolver::update(Timestep ts) {
		if (!m_active) return;
        errorSolverNotReady();


	
	}

    void PhysicsSolver::setSettings(PhysicsSettings settings) {
        m_settings = settings;
        warmUnstagedChanges();
    }

	void PhysicsSolver::add(PhysicsEntity_Ptr entity) {
		m_entity.push_back(entity);
        warmUnstagedChanges();
	}

	void PhysicsSolver::clean() {
		m_ready = false;
	}

    void PhysicsSolver::setDomain(BoundingBox aabb) {
        m_domain = aabb;
        warmUnstagedChanges();
    }

	bool PhysicsSolver::useIndexSorting() {
		return use_index_sorting;
	}

	bool PhysicsSolver::useSparseBuffer() {
		return use_sparse_buffer;
	}

	void PhysicsSolver::useIndexSorting(bool state) {
		use_index_sorting = state;
	}

	void PhysicsSolver::useSparseBuffer(bool state) {
		use_sparse_buffer = state;
		if(state) use_index_sorting = true;
	}

    void PhysicsSolver::warmUnstagedChanges() {
        if (m_ready) {
            Console::warn("PhysicsSolver") << "You've commited changes after engine initilization. Please restart the engine to stage the changes and update the physics" << Console::endl;
            return;
        }
    }

    void PhysicsSolver::errorSolverNotReady() {
        if (!m_ready) {
            m_active = false;
            Console::error("PhysicsSolver") <<
                "Physics engine is not ready." <<
                "Please init the engine before update." << Console::endl <<
                "Physics solver has been disabled" << Console::endl;
            return;
        }
    }
}