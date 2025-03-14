#include "pch.h"
#include "merlin/physics/PhysicsSolver3D.h"
#include "merlin/physics/particleSampler.h"
#include "merlin/utils/ressourceManager.h"
#include "merlin/utils/util.h"

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

    PhysicsSolver3D::PhysicsSolver3D() {
        m_particles = ParticleSystem::create("particle_system");
    }

    void PhysicsSolver3D::init() {
        Console::info("PhysicsSolver3D") << "Physics engine starting..." << Console::endl;
        clean();

        if (m_domain.isEmpty()) {
           Console::error("PhysicsSolver3D") << "Domain is empty, please set a domain before init" << Console::endl;
           return;
        }

        //-------------------------------- SCENE SCAN ---------------------------------
        Console::info("PhysicsSolver3D") << "Scanning physics scene" << Console::endl;
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
        Console::info("PhysicsSolver3D") << "Preparing solver." << Console::endl;
        m_solver = ComputeShaderLibrary::instance().getStagedComputeShader("solver");
        setConstants(*m_solver);
        m_solver->compile();
        computeThreadLayout();
        m_solver->SetWorkgroupLayout(m_pWkgCount);
        m_particles->addProgram(m_solver);
        //--------------------------------------------------------------------




        //------------------------------ BUFFERS -----------------------------
        Console::info("PhysicsSolver3D") << "Generating Buffers." << Console::endl;

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
        Console::success("PhysicsSolver3D") << "Physics engine ready." << Console::endl;
        m_ready = true;
        m_active = true;
    }
    


    void PhysicsSolver3D::generateFields() {
        m_particles->addField<glm::vec4>("position_buffer", true);
        m_particles->addField<glm::vec4>("velocity_buffer", true);
        m_particles->addField<glm::uvec4>("meta_buffer", true);
        m_particles->addField<float>("density_buffer", true);

        //-------------------------- FLUID -------------------------

        if (hasPhysics(PhysicsModifierType::FLUID)) {
            if (m_settings.fluid_solver == PressureSolver::PBF) {
                add_PBD_Buffers();
            }
            else if (m_settings.fluid_solver == PressureSolver::WCSPH) {
                if (!m_particles->hasField("pressure_buffer"))
                    m_particles->addField<float>("pressure_buffer", false);
            }
        }

        //-------------------------- RIGID_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::RIGID_BODY)) {
            if (!m_particles->hasField("rest_position_buffer"))
                m_particles->addField<glm::vec4>("rest_position_buffer", false);

            if (m_settings.rigid_solver == RigidBodySolver::SHAPE_MATCHING) {
                add_PBD_Buffers();
            }
        }


        //-------------------------- SOFT_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::SOFT_BODY)) {

            if (!m_particles->hasField("rest_position_buffer"))
                m_particles->addField<glm::vec4>("rest_position_buffer", true);

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
                    m_particles->addField<glm::mat4>("plasticity_buffer", true);
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
            m_particles->addField<float[2]>("temperature_buffer", true); // T, dT
        }
    }
    
    void PhysicsSolver3D::add_PBD_Buffers() {
        if (!m_particles->hasField("lambda_buffer"))
            m_particles->addField<float[2]>("lambda_buffer", true); //dlambda
        if (!m_particles->hasField("predicted_position_buffer"))
            m_particles->addField<glm::vec4>("predicted_position_buffer", true);
        if (!m_particles->hasField("position_correction_buffer"))
            m_particles->addField<glm::vec4>("position_correction_buffer", true);
    }

    void PhysicsSolver3D::add_MMC_Buffers() {
        if (!m_particles->hasField("F_buffer"))
            m_particles->addField<glm::mat4>("F_buffer", true);
        if (!m_particles->hasField("L_buffer"))
            m_particles->addField<glm::mat4>("L_buffer", true);
        if (!m_particles->hasField("stress_buffer"))
            m_particles->addField<glm::mat3x4>("stress_buffer", true);
    }

    void PhysicsSolver3D::generateCopyBuffer() {
        const std::map<std::string, GLuint>& structure = m_particles->getCopyBufferStructure();
        const std::map<std::string, GLuint>& glTypes = Utils::getGLTypes();

        GLuint cpy_size = 0;
        GLuint alignment = 16; // std430 alignment for vec4

        // Calculer la taille totale du buffer de copie en tenant compte de l'alignement
        for (const auto& field : structure) {
            const std::string& fieldName = field.first;
            GLuint fieldSize = Utils::getGLTypeSize(fieldName);

            if (fieldSize == -1) {
                Console::error("PhysicsSolver3D") << "Unknown field type: " << fieldName << Console::endl;
                continue;
            }

            // Aligner la taille du champ sur l'alignement std430
            cpy_size = (cpy_size + alignment - 1) & ~(alignment - 1);
            cpy_size += fieldSize;
        }

        // Ajouter le champ de copie au système de particules

        AbstractBufferObject_Ptr copyBuffer;

        //TODO generature copyBuffer

        m_particles->addField(copyBuffer, false);

        // Générer la chaîne de caractères définissant la structure du buffer pour le shader
        std::string bufferStructure = "struct CopyContent {\n";
        GLuint offset = 0;

        for (const auto& field : structure) {
            const std::string& fieldName = field.first;
            GLuint fieldSize = Utils::getGLTypeSize(fieldName);

            if (fieldSize == -1) {
                continue;
            }

            // Aligner l'offset sur l'alignement std430
            offset = (offset + alignment - 1) & ~(alignment - 1);

            // Ajouter le champ à la structure du buffer
            bufferStructure += "    " + fieldName + ";\n";
            offset += fieldSize;
        }

        bufferStructure += "};\n";

        // Ajouter la structure du buffer au shader
        m_solver->define("COPY_BUFFER_STRUCTURE", bufferStructure);
    }


    void PhysicsSolver3D::clean() {
        m_ready = false;
        use_dynamic_buffer = false;
        m_active_physics.clear();
        m_active_entities.clear();
        m_active_emitters.clear();
        m_particles->removeAllBuffer();
        m_particles->removeAllField();
    }

    void PhysicsSolver3D::reset(){
        m_elapsed_time = 0;
    }

    void PhysicsSolver3D::update(Timestep ts) {
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


    void PhysicsSolver3D::spawnParticles(int count, int phase) {
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

    void PhysicsSolver3D::setDomain(BoundingBox aabb) {
        m_domain = aabb;
        warmUnstagedChanges();
    }

    void PhysicsSolver3D::setTimestep(float dt) {
        m_settings.timestep = dt;
    }

    void PhysicsSolver3D::setSettings(PhysicsSettings3D settings) {
        m_settings = settings;
        warmUnstagedChanges();
    }

    /* Getters */

    BoundingBox PhysicsSolver3D::getDomain() {
        return m_domain;
    }

    float PhysicsSolver3D::getTimestep() {
        return m_settings.timestep.value();
    }

    PhysicsSettings3D& PhysicsSolver3D::getSettings() {
        warmUnstagedChanges();
        return m_settings;
    }


    /* Multiphysics */

	void PhysicsSolver3D::add(PhysicsEntity_Ptr entity) {
		m_entity.push_back(entity);
        warmUnstagedChanges();
	}

    void PhysicsSolver3D::addPhysics(PhysicsModifierType type) {
        m_active_physics.insert(type);
    }

    bool PhysicsSolver3D::hasPhysics(PhysicsModifierType type) {
        return m_active_physics.find(type) != m_active_physics.end();
    }
    

    /* Algorithm settings*/

	bool PhysicsSolver3D::useIndexSorting() {
		return use_index_sorting;
	}

	bool PhysicsSolver3D::useSparseBuffer() {
		return use_sparse_buffer;
	}

    bool PhysicsSolver3D::useDynamicBuffer() {
        return use_dynamic_buffer;
    }

	void PhysicsSolver3D::useIndexSorting(bool state) {
		use_index_sorting = state;
	}

	void PhysicsSolver3D::useSparseBuffer(bool state) {
		use_sparse_buffer = state;
		if(state) use_index_sorting = true;
	}


    /* Logging */

    void PhysicsSolver3D::warmUnstagedChanges() {
        if (m_ready) {
            Console::warn("PhysicsSolver3D") << "You've commited changes after engine initilization. Please restart the engine to stage the changes and update the physics" << Console::endl;
            return;
        }
    }

    void PhysicsSolver3D::errorSolverNotReady() {
        if (!m_ready) {
            m_active = false;
            Console::error("PhysicsSolver3D") <<
                "Physics engine is not ready." <<
                "Please init the engine before update." << Console::endl <<
                "Physics solver has been disabled" << Console::endl;
            return;
        }
    }

    void PhysicsSolver3D::computeThreadLayout() {
        //GPU Threading settings
        m_pWkgCount = (m_settings.particles_count + m_settings.pWkgSize - 1) / m_settings.pWkgSize; //Total number of workgroup needed

    }

    void PhysicsSolver3D::setConstants(ShaderBase& shader) {
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

    void PhysicsSolver3D::setUniforms(ShaderBase& shader) {
        
    }
}