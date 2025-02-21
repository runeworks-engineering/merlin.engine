#include "pch.h"
#include "merlin/physics/physicsSolver.h"

namespace Merlin {

    PhysicsSolver::PhysicsSolver() {

    }

    void PhysicsSolver::init() {
        // Log the start of the physics engine initialization.
        Console::info("PhysicsSolver") << "Physics engine starting..." << Console::endl;

        // -------------------------------------------------------------------------
        // 1. Create Particle Systems
        // -------------------------------------------------------------------------
        // Create the primary particle system for the simulation.
        // The maximum number of particles is assumed to be defined in your settings.
        m_particles = ParticleSystem::create("ParticleSystem", 10000);

        // Create a secondary particle system for the bins used in neighbor search.
        // This system can also be used for visualizing the load.
        m_bins = ParticleSystem::create("BinSystem", 1000);

        // -------------------------------------------------------------------------
        // 2. Compute the Simulation Domain
        // -------------------------------------------------------------------------
        //Extend the domain to include all elements
        for (const auto& entity : m_entity) {
            if (entity->isActive()) { // Assuming isActive() indicates an enabled entity.
                m_domain = BoundingBox::unionBox(m_domain, entity->getBoundingBox());
            }
        }

        // -------------------------------------------------------------------------
        // 3. Particle Sampling using Voxelized Sampling
        // -------------------------------------------------------------------------
        // Initialize a container to hold the sampled particles.

        ///////// std::vector<ParticleData> sampledParticles;

        // Define the sampling spacing (e.g., based on your particle radius).
        // This spacing determines the density of particles from the voxelization.
        float spacing = 1.0f; // Replace with settings.particleRadius * 2.0f or a similar value.

        // Iterate over each active entity and perform voxelized sampling.
        for (const auto& entity : m_entity) {
            if (entity->isActive()) {
                // Sample the entity geometry to generate particles.
                ///////// auto samples = VoxelizedSampler::sample(entity, spacing);

                // For each modifier attached to the entity, initialize the sample.
                // For example, the Heat Transfer modifier might assign an initial temperature.
                // 
                ///////// for (auto& modPair : entity->getModifiers()) {
                /////////     modPair.second->initializeSample(samples);
                ///////// }

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

	void PhysicsSolver::add(PhysicsEntity_Ptr entity) {
		m_entity.push_back(entity);
        warmUnstagedChanges();
	}

	void PhysicsSolver::clean() {
		m_ready = false;
	}

    void PhysicsSolver::setDomain(BoundingBox aabb) {
        m_domain = aabb;
        
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