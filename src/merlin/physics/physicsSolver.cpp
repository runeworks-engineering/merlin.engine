#include "pch.h"
#include "merlin/physics/physicsSolver.h"
#include "merlin/physics/particleSampler.h"
#include "merlin/utils/ressourceManager.h"

#include <set>
namespace Merlin {

    struct CopyContent {
        glm::vec4 X;
        glm::vec4 x;
        glm::vec4 p;
        glm::vec4 v;
        float density;
        float temperature;
        float pad[2];
        glm::mat4 F;
        glm::mat4 L;
        glm::mat4 plasticity;
        glm::uvec4 meta;
    };

    PhysicsSolver::PhysicsSolver() {
        m_particles = ParticleSystem::create("particle_system");
    }

    void PhysicsSolver::init() {
        Console::info("PhysicsSolver") << "Physics engine starting..." << Console::endl;
        clean();

        //-------------------------------- SCENE SCAN ---------------------------------
        Console::info("PhysicsSolver") << "Scanning physics scene" << Console::endl;
        std::vector<glm::vec4> particle_positions;
        std::vector<glm::uvec4> particle_metadata;

        int entity = 0;
        int particleID = 0;
        for (const auto& entity : m_entity) {
            if (entity->isActive()) {
                std::vector<glm::vec4> samples = entity->sample();
                m_active_entities.push_back(entity);
                int entityID = m_active_entities.size(); //0 for unused particles

                // Metas : entityID, binIndex, ID, SortedID
                for (int i = 0; i < samples.size(); i++) {
                    particle_metadata.push_back(glm::uvec4(entityID, particleID, particleID, particleID));
                    particleID++;
                }

                //Register all active physics
                for (auto& modifier : entity->getModifiers()) {
                    if (modifier.second->type() == PhysicsModifierType::EMITTER) {
                        use_dynamic_buffer = true;
                        m_active_emitters.push_back(entity);
                    }

                    addPhysics(modifier.second->type());
                }

                particle_positions.insert(particle_positions.end(), samples.begin(), samples.end());
            }
        }
        
        if(use_dynamic_buffer)
        while (particle_positions.size() < m_settings.max_particles_count) {
            particle_positions.push_back(glm::vec4(0, 0, 0, 0)); //spawn at origin
            particle_metadata.push_back(glm::uvec4(0, 0, 0, 0)); //unused
        }


        m_settings.particles_count = particle_positions.size();
        //-----------------------------------------------------------------------------



        //------------------------------- GRID --------------------------------
        m_grid = createShared<NNS>();
        m_grid->init(m_domain, m_settings.cell_width);
        //---------------------------------------------------------------------



        //------------------------------ SOLVER -------------------------------
        Console::info("PhysicsSolver") << "Preparing solver." << Console::endl;
        m_solver = ComputeShaderLibrary::instance().getStagedComputeShader("solver");
        setConstants(*m_solver);
        m_solver->compile();
        computeThreadLayout();
        m_solver->SetWorkgroupLayout(m_pWkgCount);
        m_particles->addProgram(m_solver);
        //--------------------------------------------------------------------




        //------------------------------ BUFFERS -----------------------------
        Console::info("PhysicsSolver") << "Generating Buffers." << Console::endl;

        generateFields();
        
        if(use_dynamic_buffer)
            m_particles->setInstancesCount(m_settings.max_particles_count);
        else
            m_particles->setInstancesCount(m_settings.particles_count);
        m_particles->setActiveInstancesCount(m_settings.particles_count);

        for (int i = 0; i < m_settings.particles_count; i++) {

        }

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
        m_active = true;
    }
    


    void PhysicsSolver::generateFields() {
        m_particles->addField<glm::vec4>("position_buffer");
        m_particles->addField<glm::vec4>("velocity_buffer");
        m_particles->addField<glm::uvec4>("meta_buffer");
        m_particles->addField<float[2]>("density_phase_buffer");

        //-------------------------- FLUID -------------------------

        if (hasPhysics(PhysicsModifierType::FLUID)) {
            if (m_settings.fluid_solver == PressureSolver::PBF) {
                add_PBD_Buffers();
            }
            else if (m_settings.fluid_solver == PressureSolver::WCSPH) {
                if (!m_particles->hasField("pressure_buffer"))
                    m_particles->addField<float>("pressure_buffer");
            }
        }


        //-------------------------- RIGID_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::RIGID_BODY)) {
            if (!m_particles->hasField("rest_position_buffer"))
                m_particles->addField<glm::vec4>("rest_position_buffer");

            if (m_settings.rigid_solver == RigidBodySolver::SHAPE_MATCHING) {
                add_PBD_Buffers();
            }
        }



        //-------------------------- SOFT_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::SOFT_BODY)) {

            if (!m_particles->hasField("rest_position_buffer"))
                m_particles->addField<glm::vec4>("rest_position_buffer");

            if (m_settings.soft_solver == SoftBodySolver::PBD_WDC) {
                add_PBD_Buffers();
            }
            else if (m_settings.soft_solver == SoftBodySolver::MMC_PBD) {
                add_PBD_Buffers();
                add_MMC_Buffers();
            }
            else if (m_settings.soft_solver == SoftBodySolver::MMC_PBD) {
                add_MMC_Buffers();
            }

            if (hasPhysics(PhysicsModifierType::PLASTICITY)) {
                if (!m_particles->hasField("plasticity_buffer"))
                    m_particles->addField<glm::mat4>("plasticity_buffer");
            }

        }


        //-------------------------- GRANULAR_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::GRANULAR_BODY)) {
            if (m_settings.granular_solver == GranularBodySolver::PBD_DC) {
                add_PBD_Buffers();
            }
        }


        //-------------------------- HEAT_TRANSFER -------------------------

        if (hasPhysics(PhysicsModifierType::HEAT_TRANSFER)) {
            m_particles->addField<float[2]>("temperature_buffer"); // T, dT
        }
    }
    
    void PhysicsSolver::add_PBD_Buffers() {
        if (!m_particles->hasField("lambda_buffer"))
            m_particles->addField<float[2]>("lambda_buffer"); //dlambda
        if (!m_particles->hasField("predicted_position_buffer"))
            m_particles->addField<glm::vec4>("predicted_position_buffer");
        if (!m_particles->hasField("position_correction_buffer"))
            m_particles->addField<glm::vec4>("position_correction_buffer");
    }

    void PhysicsSolver::add_MMC_Buffers() {
        if (!m_particles->hasField("F_buffer"))
            m_particles->addField<glm::mat4>("F_buffer");
        if (!m_particles->hasField("L_buffer"))
            m_particles->addField<glm::mat4>("L_buffer");
        if (!m_particles->hasField("stress_buffer"))
            m_particles->addField<glm::mat3x4>("stress_buffer");
    }

    void PhysicsSolver::clean() {
        m_ready = false;
        use_dynamic_buffer = false;
        m_active_physics.clear();
        m_active_entities.clear();
        m_active_emitters.clear();
        m_particles->removeAllBuffer();
        m_particles->removeAllField();
    }

    void PhysicsSolver::reset(){
        m_elapsed_time = 0;
    }

    void PhysicsSolver::update(Timestep ts) {
		if (!m_active) return;
        errorSolverNotReady();

        m_elapsed_time += m_settings.timestep.value();

        for (auto emitter : m_active_emitters) {
            shared<Emitter> e = emitter->getModifier<Emitter>();

            if (m_elapsed_time - e->lastSpawnTime() > e->emitterDelay() && e->isActive()) {
                spawnParticles(e->count(), e->phase());
                e->setLastSpawnTime(m_elapsed_time);
            }
        }

        // Update emmiters & spawn particles

        m_grid->sort(m_solver);


        /*
        GPU_PROFILE(solver_substep_time,
            for (int i = 0; i < settings.solver_substep; i++) {
                if (integrate) {
                    solver->execute(2);


                }
                if (integrate) {
                    GPU_PROFILE(jacobi_time,
                        for (int j = 0; j < settings.solver_iteration; j++) {
                            solver->execute(3);
                            solver->execute(4);
                        }
                            )
                        solver->execute(5);

                }
            }
         )
        */



	}


    void PhysicsSolver::spawnParticles(int count, int phase) {
        int old_particles_count = m_settings.particles_count;
        m_settings.particles_count += count;
        computeThreadLayout();
        m_solver->SetWorkgroupLayout(m_pWkgCount);
        m_particles->setActiveInstancesCount(m_settings.particles_count);

        m_solver->use();
        m_solver->setUInt("numEmitter", count);
        m_solver->setUInt("numParticles", old_particles_count);
        m_solver->setUInt("emitterPhase", phase);

        m_solver->execute(SPAWN);

        m_solver->setUInt("numEmitter", 0);
        m_solver->setUInt("numParticles", m_settings.particles_count);

        Shader_Ptr shader = m_particles->getShader();
        if (shader) {
            shader->use();
            shader->setUInt("numParticles", m_settings.particles_count);
        }
        //TODO update ISoSurface count as well
    }

    /* Setters */

    void PhysicsSolver::setDomain(BoundingBox aabb) {
        m_domain = aabb;
        warmUnstagedChanges();
    }

    void PhysicsSolver::setTimestep(float dt) {
        m_settings.timestep = dt;
    }

    void PhysicsSolver::setSettings(PhysicsSettings settings) {
        m_settings = settings;
        warmUnstagedChanges();
    }

    /* Getters */

    BoundingBox PhysicsSolver::getDomain() {
        return m_domain;
    }

    float PhysicsSolver::getTimestep() {
        return m_settings.timestep.value();
    }

    PhysicsSettings& PhysicsSolver::getSettings() {
        warmUnstagedChanges();
        return m_settings;
    }


    /* Multiphysics */

	void PhysicsSolver::add(PhysicsEntity_Ptr entity) {
		m_entity.push_back(entity);
        warmUnstagedChanges();
	}

    void PhysicsSolver::addPhysics(PhysicsModifierType type) {
        m_active_physics.insert(type);
    }

    bool PhysicsSolver::hasPhysics(PhysicsModifierType type) {
        return m_active_physics.find(type) != m_active_physics.end();
    }
    

    /* Algorithm settings*/

	bool PhysicsSolver::useIndexSorting() {
		return use_index_sorting;
	}

	bool PhysicsSolver::useSparseBuffer() {
		return use_sparse_buffer;
	}

    bool PhysicsSolver::useDynamicBuffer() {
        return use_dynamic_buffer;
    }

	void PhysicsSolver::useIndexSorting(bool state) {
		use_index_sorting = state;
	}

	void PhysicsSolver::useSparseBuffer(bool state) {
		use_sparse_buffer = state;
		if(state) use_index_sorting = true;
	}


    /* Logging */

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

    void PhysicsSolver::computeThreadLayout() {
        //GPU Threading settings
        m_pWkgCount = (m_settings.particles_count + m_settings.pWkgSize - 1) / m_settings.pWkgSize; //Total number of workgroup needed

    }

    void PhysicsSolver::setConstants(ShaderBase& shader) {
        shader.setConstVec3("cst_domain", m_domain.size);
        shader.setConstVec3("cst_halfdomain", m_domain.size * glm::vec3(0.5));
        shader.setConstVec3("cst_boundaryMin", m_domain.min);
        shader.setConstVec3("cst_boundaryMax", m_domain.max);

        shader.setConstFloat("cst_particleRadius", m_settings.particle_radius);
        shader.setConstFloat("cst_smoothingRadius", m_settings.smoothing_radius);

        shader.setConstFloat("cst_binSize", m_grid->getCellSize());
        shader.setConstUVec3("cst_binMax", m_grid->getBound());
        shader.setConstUInt("cst_binCount", m_grid->getBinCount());

        shader.define("PTHREAD", std::to_string(m_settings.pWkgSize));
    }

    void PhysicsSolver::setUniforms(ShaderBase& shader) {
        
    }
}