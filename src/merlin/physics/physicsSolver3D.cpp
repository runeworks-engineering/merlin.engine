#include "pch.h"
#include "merlin/physics/PhysicsSolver3D.h"
#include "merlin/physics/particleSampler.h"
#include "merlin/utils/ressourceManager.h"
#include "merlin/utils/util.h"
#include "merlin/memory/bindingPointManager.h"
#include "merlin/utils/primitives.h"
#include "merlin/memory/acb.h"

#include <set>
namespace Merlin {

    PhysicsSolver3D::PhysicsSolver3D() {
        m_particles = ParticleSystem::create("particle_system");
        m_particles->setDisplayMode(ParticleSystemDisplayMode::POINT_SPRITE_SHADED);
    }

    void PhysicsSolver3D::init() {
        Console::printBoldSeparator();
        Console::info("PhysicsSolver3D") << "Physics engine starting..." << Console::endl;
        //Console::printSeparator();
        clean();

        if (m_domain.isEmpty()) {
           Console::error("PhysicsSolver3D") << "Domain is empty, please set a domain before init" << Console::endl;
           return;
        }


        int particleID = 0;

        //-------------------------------- SCENE SCAN ---------------------------------
        Console::info("PhysicsSolver3D") << "Scanning physics scene" << Console::endl;
        Console::printSeparator();
        scanScene();
        Console::printSeparator();
        //-----------------------------------------------------------------------------


        //-------------------------- DEFAULT BUFFER SAMPLING---------------------------
        //Sampling entities
        for (const auto& entity : m_active_entities) {
            BoundingBox union_check = BoundingBox::unionBox(m_domain, entity.second->getBoundingBox());
            glm::vec3 bound_check = union_check.size;

            if (bound_check.x > m_domain.size.x || bound_check.y > m_domain.size.y || bound_check.z > m_domain.size.z) {
                Console::error("PhysicsSolver") << "Entity (" << entity.second->name() << ") is outside simulation domain" << Console::endl;
                Console::error("PhysicsSolver") << "-> recomended BoundingBox : " << Console::endl <<
                    "    - min(" << union_check.min << ")" << Console::endl <<
                    "    - max(" << union_check.max << ")" << Console::endl <<
                    "    - size(" << union_check.size << ")" << Console::endl;
            }

            std::vector<glm::vec4> samples = entity.second->sample();

            int entityID = m_active_entities.size(); //0 for unused particles

            for (int i = 0; i < samples.size(); i++) {
                cpu_meta_buffer.push_back(glm::uvec4(entityID, entity.second->getPhase(), particleID, particleID));
                particleID++;
            }

            cpu_position_buffer.insert(cpu_position_buffer.end(), samples.begin(), samples.end());
        }
        
        //Sampling emitters
        for (const auto& emitter : m_active_emitters) {
            std::vector<glm::vec4> samples = emitter.second->sample();
            cpu_emitter_position_buffer.insert(cpu_emitter_position_buffer.end(), samples.begin(), samples.end());
        }

        //Populate void particles for emitters
        if(use_dynamic_buffer)
        while (cpu_position_buffer.size() < m_settings.max_particles_count.value()) {
            cpu_position_buffer.push_back(glm::vec4(0, 0, 0, 0)); //spawn at origin
            cpu_meta_buffer.push_back(glm::uvec4(0, 0, 0, 0)); //unused
        }
        
        //Active partile count
        m_settings.particles_count = cpu_position_buffer.size();
        m_settings.initial_particles_count = cpu_position_buffer.size();

        if (use_dynamic_buffer)
            m_particles->setInstancesCount(m_settings.max_particles_count.value());
        else
            m_particles->setInstancesCount(m_settings.particles_count.value());
        m_particles->setActiveInstancesCount(m_settings.particles_count.value());
        //-----------------------------------------------------------------------------






        //------------------------------- GRID --------------------------------
        Console::printSeparator();
        Console::info("PhysycsSolver") << "Preparing NNS Grid" << Console::endl;
        Console::printSeparator();
        m_grid = createShared<NNS>();
        m_grid->init(m_domain, m_settings.cell_width.value());
        //---------------------------------------------------------------------



        //----------------------------- SHADERS -------------------------------
        Console::printSeparator();
        Console::info("PhysicsSolver3D") << "Preparing solver shaders." << Console::endl;
        Console::printSeparator();
        //m_solver = ComputeShaderLibrary::instance().getStagedComputeShader("solver");
        //m_filter = ComputeShader::create("filter", "assets/common/shaders/physics/filter.comp", false);

        pshader = Shader::create("particle",
            "assets/common/shaders/physics/graphics/particle.vert",
            "assets/common/shaders/physics/graphics/particle.frag", "", false);

        bshader = Shader::create("bin",
            "assets/common/shaders/physics/graphics/bin.vert",
            "assets/common/shaders/physics/graphics/bin.frag", "", false);

        //setConstants(*m_solver);
       // setConstants(*m_filter);
        setConstants(*pshader);
        setConstants(*bshader);

        //m_solver->compile();
        //m_filter->compile();
        pshader->compile();
        bshader->compile();

        m_particles->setShader(pshader);
        m_grid->setShader(bshader);

        computeThreadLayout();
        // m_solver->SetWorkgroupLayout(m_pWkgCount);
        // m_filter->SetWorkgroupLayout(m_pWkgCount);

        //setUniforms(*m_solver);
        //setUniforms(*m_filter);
        setUniforms(*pshader);
        setUniforms(*bshader);



        //----------------------------- PIPELINE -----------------------------
        if (!m_pipeline) {
            Console::info("PhysicsSolver") << "No pipeline set, generating default Pipeline" << Console::endl;
            m_pipeline = createShared<PhysicsPipeline>("solver");
            
        }
        
        //m_pipeline
        //m_pipeline->

        //--------------------------------------------------------------------

        //------------------------------ BUFFERS -----------------------------
        Console::printSeparator();
        Console::info("PhysicsSolver3D") << "Generating Buffers..." << Console::endl;
        Console::printSeparator();

        //generateFields();
        if (m_pipeline) {
            m_pipeline->initialize();
            m_pipeline->addBuffer(m_grid->getBuffer());
            m_pipeline->addField<glm::vec4>("position_buffer", true);
            m_particles->setPositionBuffer(m_pipeline->getField("position_buffer"));
            m_pipeline->link(pshader->name(), m_particles->getPositionBuffer()->name());
        }
        reset();
        
        // Mark the physics engine as ready.
        Console::printSeparator();
        Console::success("PhysicsSolver3D") << "Physics engine ready." << Console::endl;
        Console::printBoldSeparator();
        m_ready = true;
        m_active = true;
    }

    //Old Implementation
    /*
    
    void PhysicsSolver3D::uploadFields() {
        Console::info() << "Uploading buffer on device..." << Console::endl;

        if (!m_pipeline) return;

        //Console::printProgress(0);
        m_pipeline->writeField("meta_buffer", cpu_meta_buffer);         //Console::printProgress(0.05);
        m_pipeline->clearField("filter_buffer");                        //Console::printProgress(0.1);
        m_pipeline->writeField("position_buffer", cpu_position_buffer); //Console::printProgress(0.15);


        /*

        m_pipeline->writeField("velocity_buffer", cpu_velocity_buffer); Console::printProgress(0.20);
        m_pipeline->writeField("density_buffer", cpu_density_buffer);   Console::printProgress(0.25);

      

        //-------------------------- FLUID -------------------------
        bool PDB_uploaded = false;
        bool MMC_uploaded = false;
        bool rest_pos_uploaded = false;


        if (hasPhysics(PhysicsModifierType::FLUID)) {
            if (m_settings.fluid_solver == PressureSolver::PBF) {
                m_pipeline->writeField("lambda_buffer", cpu_lambda_buffer); //dlambda
                m_pipeline->writeField("predicted_position_buffer", cpu_position_buffer);
                m_pipeline->writeField("position_correction_buffer", cpu_correction_position_buffer);
                PDB_uploaded = true;
            }
            else if (m_settings.fluid_solver == PressureSolver::WCSPH) {
                m_pipeline->clearField("pressure_buffer");
            }
        }Console::printProgress(0.40);

        //-------------------------- RIGID_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::RIGID_BODY)) {
            if (!rest_pos_uploaded) {
                m_pipeline->writeField("rest_position_buffer", cpu_position_buffer);
                rest_pos_uploaded = true;
            }

            if (m_settings.rigid_solver == RigidBodySolver::SHAPE_MATCHING && !PDB_uploaded) {
                m_pipeline->writeField("lambda_buffer", cpu_lambda_buffer); //dlambda
                m_pipeline->writeField("predicted_position_buffer", cpu_position_buffer);
                m_pipeline->writeField("position_correction_buffer", cpu_correction_position_buffer);
                PDB_uploaded = true;
            }
        }Console::printProgress(0.50);

        //-------------------------- SOFT_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::SOFT_BODY)) {

            if (!rest_pos_uploaded) {
                m_pipeline->writeField("rest_position_buffer", cpu_position_buffer);
                rest_pos_uploaded = true;
            }

            if (m_settings.soft_solver == SoftBodySolver::PBD_WDC && !PDB_uploaded) {
                m_pipeline->writeField("lambda_buffer", cpu_lambda_buffer); //dlambda
                m_pipeline->writeField("predicted_position_buffer", cpu_position_buffer);
                m_pipeline->writeField("position_correction_buffer", cpu_correction_position_buffer);
                PDB_uploaded = true;
            }
            else if (m_settings.soft_solver == SoftBodySolver::MMC_PBD) {
                if (!PDB_uploaded) {
                    m_pipeline->writeField("lambda_buffer", cpu_lambda_buffer); //dlambda
                    m_pipeline->writeField("predicted_position_buffer", cpu_position_buffer);
                    m_pipeline->writeField("position_correction_buffer", cpu_correction_position_buffer);
                    PDB_uploaded = true;
                }
                if (!MMC_uploaded) {
                    m_pipeline->clearField("F_buffer");
                    m_pipeline->clearField("L_buffer");
                    m_pipeline->clearField("stress_buffer");
                    //m_particles->writeField("stress_buffer", cpu_stress_buffer);
                    MMC_uploaded = true;
                }
            }
            else if (m_settings.soft_solver == SoftBodySolver::MMC_PBD) {
                if (!MMC_uploaded) {
                    m_pipeline->clearField("F_buffer");
                    m_pipeline->clearField("L_buffer");
                    m_pipeline->clearField("stress_buffer");
                    //m_particles->writeField("stress_buffer", cpu_stress_buffer);
                    MMC_uploaded = true;
                }
            }Console::printProgress(0.70);

            if (hasPhysics(PhysicsModifierType::PLASTICITY)) {
                m_pipeline->clearField("plasticity_buffer");
            }

        }

        //-------------------------- GRANULAR_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::GRANULAR_BODY)) {
            if (m_settings.granular_solver == GranularBodySolver::PBD_DC) {
                if (!PDB_uploaded) {
                    m_pipeline->writeField("lambda_buffer", cpu_lambda_buffer); //dlambda
                    m_pipeline->writeField("predicted_position_buffer", cpu_position_buffer);
                    m_pipeline->writeField("position_correction_buffer", cpu_correction_position_buffer);
                    PDB_uploaded = true;
                }
            }
        }Console::printProgress(0.9);

        //-------------------------- HEAT_TRANSFER -------------------------

        if (hasPhysics(PhysicsModifierType::HEAT_TRANSFER)) {
            m_pipeline->writeField("temperature_buffer", cpu_temperature_buffer);
        }Console::printProgress(1.0);
        Console::print() << Console::endl;
        
    }
    */
    
    /*
    void PhysicsSolver3D::generateFields() {

        // Meta-data layout : 
        // meta[i].x = entityID
        // meta[i].y = phase/flags (bit-packed)
        // meta[i].z = particleID
        // meta[i].w = sortedID
        if (!m_pipeline) return;

        m_pipeline->addField<glm::uvec4>("meta_buffer", true);
        m_pipeline->addField<GLuint>("filter_buffer", false); //To select particle sub-group
        m_pipeline->addField<glm::vec4>("position_buffer", true);
        m_pipeline->addField<glm::vec4>("velocity_buffer", true);
        m_pipeline->addField<float>("density_buffer", true);
       
        m_pipeline->link(pshader->name(), "meta_buffer");
        m_pipeline->link(pshader->name(), "filter_buffer");
        m_pipeline->link(pshader->name(), "position_buffer");
        m_pipeline->link(pshader->name(), "velocity_buffer");
        m_pipeline->link(pshader->name(), "density_buffer");

        //-------------------------- FLUID -------------------------

        if (hasPhysics(PhysicsModifierType::FLUID)) {
            if (m_settings.pressureSolver == PressureSolver::PBF) {
                add_PBD_Buffers();
            }
            else if (m_settings.pressureSolver == PressureSolver::WCSPH) {
                if (!m_pipeline->hasField("pressure_buffer"))
                    m_pipeline->addField<float>("pressure_buffer", false);
                m_pipeline->link(pshader->name(), "pressure_buffer");
            }
        }

        //-------------------------- RIGID_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::RIGID_BODY)) {
            if (!m_pipeline->hasField("rest_position_buffer"))
                m_pipeline->addField<glm::vec4>("rest_position_buffer", false);

            if (m_settings.rigidBodySolver == RigidBodySolver::SHAPE_MATCHING) {
                add_PBD_Buffers();
            }
        }

        //-------------------------- SOFT_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::SOFT_BODY)) {

            if (!m_pipeline->hasField("rest_position_buffer"))
                m_pipeline->addField<glm::vec4>("rest_position_buffer", true);

            if (m_settings.softBodySolver == SoftBodySolver::PBD_WDC) {
                add_PBD_Buffers();
            }
            else if (m_settings.softBodySolver == SoftBodySolver::MMC_PBD) {
                add_PBD_Buffers();
                add_MMC_Buffers();
            }
            else if (m_settings.softBodySolver == SoftBodySolver::MMC_PBD) {
                add_MMC_Buffers();
            }

            if (hasPhysics(PhysicsModifierType::PLASTICITY)) {
                if (!m_pipeline->hasField("plasticity_buffer"))
                    m_pipeline->addField<glm::mat4>("plasticity_buffer", true);
                m_pipeline->link(pshader->name(), "plasticity_buffer");
            }

        }

        //-------------------------- GRANULAR_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::GRANULAR_BODY)) {
            if (m_settings.granularBodySolver == GranularBodySolver::PBD_DC) {
                add_PBD_Buffers();
            }
        }

        //-------------------------- HEAT_TRANSFER -------------------------

        if (hasPhysics(PhysicsModifierType::HEAT_TRANSFER)) {
            m_pipeline->addField<float[2]>("temperature_buffer", true); // T, dT
        }
    }

    */

    /*

    void PhysicsSolver3D::add_PBD_Buffers() {
        if (!m_pipeline->hasField("lambda_buffer"))
            m_pipeline->addField<float[2]>("lambda_buffer", true); //dlambda
        if (!m_pipeline->hasField("predicted_position_buffer"))
            m_pipeline->addField<glm::vec4>("predicted_position_buffer", true);
        if (!m_pipeline->hasField("position_correction_buffer"))
            m_pipeline->addField<glm::vec4>("position_correction_buffer", true);

        m_pipeline->link(pshader->name(), "lambda_buffer");
    }

    void PhysicsSolver3D::add_MMC_Buffers() {
        if (!m_pipeline->hasField("F_buffer"))
            m_pipeline->addField<glm::mat4>("F_buffer", true);
        if (!m_pipeline->hasField("L_buffer"))
            m_pipeline->addField<glm::mat4>("L_buffer", true);
        if (!m_pipeline->hasField("stress_buffer"))
            m_pipeline->addField<glm::mat3x4>("stress_buffer", true);

        m_pipeline->link(pshader->name(), "stress_buffer");
    }

    */

    /*
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
    */


    void PhysicsSolver3D::generateDefaultPipeline(){
        if (!m_pipeline) return;
        //-------------------------- FLUID -------------------------

        if (hasPhysics(PhysicsModifierType::FLUID)) {
            if (m_settings.pressureSolver == PressureSolver::PBF) {

                //StagedComputeShader_Ptr pbf_kernels

                //PhysicsPipelineStep_Ptr step;
                //step = PhysicsPipelineStep::create("PBF_predict");
                //step->setIteration(1);
                //step->setProgram();
                //m_pipeline->addStep();
            }
            else if (m_settings.pressureSolver == PressureSolver::WCSPH) {

            }
        }

        //-------------------------- RIGID_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::RIGID_BODY)) {
            if (m_settings.rigidBodySolver == RigidBodySolver::SHAPE_MATCHING) {

            }
        }

        //-------------------------- SOFT_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::SOFT_BODY)) {
            if (m_settings.softBodySolver == SoftBodySolver::PBD_WDC) {

            }
            else if (m_settings.softBodySolver == SoftBodySolver::MMC_PBD) {

            }
            else if (m_settings.softBodySolver == SoftBodySolver::MMC_PBD) {

            }

            if (hasPhysics(PhysicsModifierType::PLASTICITY)) {

            }

        }

        //-------------------------- GRANULAR_BODY -------------------------

        if (hasPhysics(PhysicsModifierType::GRANULAR_BODY)) {
            if (m_settings.granularBodySolver == GranularBodySolver::PBD_DC) {
                        
            }
        }

        //-------------------------- HEAT_TRANSFER -------------------------

        if (hasPhysics(PhysicsModifierType::HEAT_TRANSFER)) {

        }
    }

    void PhysicsSolver3D::clean() {
        m_ready = false;
        use_dynamic_buffer = false;
        m_active_physics.clear();
        m_active_entities.clear();
        m_active_emitters.clear();
        if (m_pipeline) {
            m_pipeline->removeAllBuffer();
            m_pipeline->removeAllField();
        }
        BindingPointManager::instance().resetBindings();
    }

    void PhysicsSolver3D::reset(){
        m_elapsed_time = 0;
        
        BindingPointManager::instance().resetBindings();

        m_settings.particles_count.value() = m_settings.initial_particles_count;

        if (use_dynamic_buffer)
            m_particles->setInstancesCount(m_settings.max_particles_count.value());
        else
            m_particles->setInstancesCount(m_settings.particles_count.value());
        m_particles->setActiveInstancesCount(m_settings.particles_count.value());

        //uploadFields();

        //m_solver->use();
        //m_solver->execute(SolverStages::INIT);
    }

    void PhysicsSolver3D::update(Timestep ts) {
		if (!m_active) return;
        errorSolverNotReady();

        if(use_fixed_timestep)
            m_elapsed_time += m_settings.timestep.value();
        else
            m_elapsed_time += ts.getSeconds();

        for (const auto& emitter : m_active_emitters) {
            shared<Emitter> e = emitter.second->getModifier<Emitter>();

            if (m_elapsed_time - e->lastSpawnTime() > e->emitterDelay() && e->isActive()) {
                spawnParticles(e->count(), e->phase());
                e->setLastSpawnTime(m_elapsed_time);
            }
        }

        // Update emmiters & spawn particles
       // m_grid->sort(m_solver);
        for (int i = 0; i < m_settings.solver_substep; i++) {
           // m_solver->execute(SolverStages::SOLVER_STEP_1);
            for (int j = 0; j < m_settings.solver_iteration; j++) {
               // m_solver->execute(SolverStages::SOLVER_STEP_2);
                //m_solver->execute(SolverStages::SOLVER_STEP_3);
            }
            //m_solver->execute(SolverStages::SOLVER_STEP_4);
        }
	}


    void PhysicsSolver3D::spawnParticles(int count, int phase) {
        /*
        int old_particles_count = m_settings.particles_count.value();
        m_settings.particles_count.value() += count;
        computeThreadLayout();
        m_solver->SetWorkgroupLayout(m_pWkgCount);
        m_particles->setActiveInstancesCount(m_settings.particles_count.value());

        m_solver->use();
        m_solver->setUInt("u_numEmitter", count);
        m_solver->setUInt("u_numParticles", old_particles_count);
        m_solver->setUInt("u_emitterPhase", phase);

        m_solver->execute(SPAWN);

        m_solver->setUInt("u_numEmitter", 0);
        m_settings.particles_count.sync(*m_solver);

        Shader_Ptr shader = m_particles->getShader();
        if (shader) {
            shader->use();
            shader->setUInt("u_numParticles", m_settings.particles_count.value());
        }

        */


        //TODO update ISoSurface count as well
    }

    void PhysicsSolver3D::attachGraphics(){
        if(m_pipeline)
            m_pipeline->solveLink(pshader);

        if(m_grid)
            m_grid->solveLink(bshader);
    }

    void PhysicsSolver3D::detachGraphics(){
        if(m_pipeline)
            m_pipeline->detach(pshader);
        if (m_grid)
            m_grid->detach(bshader);
    }

    ParticleSystem_Ptr PhysicsSolver3D::getParticles(){
        return m_particles;
    }

    ParticleSystem_Ptr PhysicsSolver3D::getBins(){
        return m_grid->getParticleSystem();
    }

    std::vector<PhysicsEntity_Ptr> PhysicsSolver3D::getEntities()
    {
		return m_entity;
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
        scanScene();
	}

    void PhysicsSolver3D::addPhysics(PhysicsModifierType type) {
        m_active_physics.insert(type);
    }

    bool PhysicsSolver3D::hasPhysics(PhysicsModifierType type) {
        return m_active_physics.find(type) != m_active_physics.end();
    }
     

    /* Algorithm settings*/

	bool PhysicsSolver3D::useIndexSorting() const {
		return use_index_sorting;
	}

	bool PhysicsSolver3D::useSparseBuffer() const {
		return use_sparse_buffer;
	}

    bool PhysicsSolver3D::useDynamicBuffer() const {
        return use_dynamic_buffer;
    }

	void PhysicsSolver3D::useIndexSorting(bool state) {
		use_index_sorting = state;
        warmUnstagedChanges();
	}

	void PhysicsSolver3D::useSparseBuffer(bool state) {
		use_sparse_buffer = state;
		if(state) use_index_sorting = true;
        warmUnstagedChanges();
	}

    void PhysicsSolver3D::useFixedTimeStep(bool state){
        use_fixed_timestep = state;
        warmUnstagedChanges();
    }

    void PhysicsSolver3D::scanScene() {
        
        warmUnstagedChanges();


        for (const auto& entity : m_entity) {
            if (entity->isActive()) {
                const std::vector<glm::vec4>& samples = entity->sample();
                //Register all active physics
                for (auto& modifier : entity->getModifiers()) {
                    addPhysics(modifier.second->type()); //Gather physics
                }
                if (!entity->hasModifier(PhysicsModifierType::EMITTER)) {
                    if(m_active_entities.find(entity->id()) == m_active_entities.end())
                        m_active_entities[entity->id()] = entity; //Gather entities
                }
                else {
                    use_dynamic_buffer = true;
                    if (m_active_emitters.find(entity->id()) == m_active_emitters.end())
                        m_active_emitters[entity->id()] = entity;
                }
            }
        }

    }

    float PhysicsSolver3D::getTime() const {
        return m_elapsed_time;
    }

    void PhysicsSolver3D::onRenderMenu(){
        ImGui::Text("Solver properties");

        ImGui::Text("Current Time: %f s", m_elapsed_time);
        if (ImGui::Checkbox("Use fixed timestep", &use_fixed_timestep)) {
            useFixedTimeStep(use_fixed_timestep);
        }
        
        if (ImGui::TreeNode("Entities")) {
            ImGui::Text("Active: %s", m_active ? "true" : "false");
            ImGui::Text("Ready: %s", m_ready ? "true" : "false");
            ImGui::Text("Particle count: %i", cpu_position_buffer.size());
            ImGui::Text("Emitter Particle count : %i", cpu_emitter_position_buffer.size());

            ImGui::Text("Total Entities: %i", m_entity.size());
            ImGui::Text("Total Active Entities: %i", m_active_entities.size());
            ImGui::Text("Total Active Emitters: %i", m_active_emitters.size());
            ImGui::TreePop();
        }
       
        if (ImGui::TreeNode("Global settings")) {

            //m_grid->onRenderMenu();
            BoundingBox m_domain;
            ImGui::InputFloat3("Domain: ", &m_domain.size.x);

            ImGui::InputFloat("Particle radius: ", &m_settings.particle_radius.value());
            ImGui::InputFloat("Smoothing radius: ", &m_settings.smoothing_radius.value());
            ImGui::InputFloat("Bin size: ", &m_settings.cell_width.value());
            ImGui::InputFloat("Timestep: ", &m_settings.timestep.value());
            ImGui::InputFloat("Particle mass: ", &m_settings.particle_mass.value());

            

            //m_pWkgCount
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Algorithm")) {
            ImGui::InputInt("Solver substep: %s", &m_settings.solver_substep);
            ImGui::InputInt("Solver Iteration: %s", &m_settings.solver_iteration);
            ImGui::InputFloat("Solver OverRelaxation: %s", &m_settings.overRelaxation);

            if (ImGui::Checkbox("Use index sorting", &use_index_sorting)) {
                useIndexSorting(use_index_sorting);
            }

            if (ImGui::Checkbox("Use sparse buffer", &use_sparse_buffer)) {
                useSparseBuffer(use_sparse_buffer);
            }

            ImGui::BeginDisabled();
            ImGui::Checkbox("Use dynamic buffer", &use_dynamic_buffer);
            ImGui::EndDisabled();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Physics")) {
            for (const auto& physics : m_active_physics) {
                if (ImGui::TreeNode((PhysicsModifier::toString(physics) + " properties").c_str())) {

                    const char* pressureSolvers[] = { "WSPH", "PBF" };
                    int pressureSolverIndex = (int)m_settings.pressureSolver;

                    const char* viscositySolvers[] = { "XSPH" };
                    int viscositySolverIndex = (int)m_settings.viscositySolver;

                    const char* softBodySolver[] = { "MMC_SPH", "PBD_WDC", "MMC_PBD" };
                    int softBodySolverIndex = (int)m_settings.softBodySolver;

                    const char* rigidBodySolver[] = { "SHAPE_MATCHING" };
                    int rigidBodySolverIndex = (int)m_settings.rigidBodySolver;

                    const char* granularBodySolver[] = { "DEM", "PBD_DC" };
                    int granularBodySolverIndex = (int)m_settings.granularBodySolver;

                    switch (physics) {
                    case PhysicsModifierType::FLUID:
                        if (ImGui::Combo("Pressure Solver", &pressureSolverIndex, pressureSolvers, IM_ARRAYSIZE(pressureSolvers))) {
                            m_settings.pressureSolver = (PressureSolver)pressureSolverIndex;
                        }

                        if (ImGui::Combo("Viscosity Solver", &viscositySolverIndex, viscositySolvers, IM_ARRAYSIZE(viscositySolvers))) {
                            m_settings.viscositySolver = (ViscositySolver)viscositySolverIndex;
                        }
                        break;

                    case PhysicsModifierType::SOFT_BODY:
                        if (ImGui::Combo("Soft Body Solver", &softBodySolverIndex, softBodySolver, IM_ARRAYSIZE(softBodySolver))) {
                            m_settings.softBodySolver = (SoftBodySolver)softBodySolverIndex;
                        }
                        break;

                    case PhysicsModifierType::RIGID_BODY:
                        if (ImGui::Combo("Rigid Body Solver", &rigidBodySolverIndex, rigidBodySolver, IM_ARRAYSIZE(rigidBodySolver))) {
                            m_settings.rigidBodySolver = (RigidBodySolver)rigidBodySolverIndex;
                        }
                        break;

                    case PhysicsModifierType::GRANULAR_BODY:
                        if (ImGui::Combo("Granular Body Solver", &granularBodySolverIndex, granularBodySolver, IM_ARRAYSIZE(granularBodySolver))) {
                            m_settings.granularBodySolver = (GranularBodySolver)granularBodySolverIndex;
                        }
                        break;

                    case PhysicsModifierType::HEAT_TRANSFER:

                        break;
                    default:
                        break;
                    }


                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        
        if (ImGui::TreeNode("Grid")) {

            //m_grid->onRenderMenu();

            ImGui::TreePop();
        }

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
        m_pWkgCount = (m_settings.particles_count.value() + m_settings.pWkgSize - 1) / m_settings.pWkgSize; //Total number of workgroup needed

    }

    void PhysicsSolver3D::gatherParticleGroup(){
        AtomicCounterBuffer atomicCounterBuffer;
        atomicCounterBuffer.allocate(sizeof(GLuint), Merlin::BufferUsage::DynamicDraw);
        GLuint bufferID = atomicCounterBuffer.id();
        GLuint bindingPoint = Merlin::BindingPointManager::instance().allocateBindingPoint(Merlin::BufferTarget::Atomic_Counter_Buffer, bufferID);
        atomicCounterBuffer.bindBase(bindingPoint);

        GLuint initialValue = 0;
        atomicCounterBuffer.writeBuffer(sizeof(GLuint), &initialValue);
        
        //m_filter->attach(atomicCounterBuffer);
        m_filter->use();
        //m_filter->dispatch(m_settings.particles_count);

        // Lire la valeur du tampon
        GLuint counterValue;
        atomicCounterBuffer.readBuffer(sizeof(GLuint), &counterValue);
        std::cout << "Counter Value: " << counterValue << std::endl;


    }

    void PhysicsSolver3D::setConstants(ShaderBase& shader) {
        shader.setConstVec3("cst_domain", m_domain.size);
        shader.setConstVec3("cst_halfdomain", m_domain.size * glm::vec3(0.5));
        shader.setConstVec3("cst_boundaryMin", m_domain.min);
        shader.setConstVec3("cst_boundaryMax", m_domain.max);

        m_settings.particle_radius.set(shader);
        m_settings.smoothing_radius.set(shader);

        shader.setConstFloat("cst_binSize", m_grid->getCellSize());
        shader.setConstUVec3("cst_binMax", m_grid->getBound());
        shader.setConstUInt("cst_binCount", m_grid->getBinCount());

        shader.define("PTHREAD", std::to_string(m_settings.pWkgSize));
    }

    void PhysicsSolver3D::setUniforms(ShaderBase& shader) {
        shader.use();

        //settings.timestep.sync(*solver);
        m_settings.particle_mass.sync(shader);
        m_settings.timestep.sync(shader);
        
        m_settings.particles_count.sync(shader);
        m_settings.max_particles_count.sync(shader);
        //shader.setInt("u_numEmitter", m_settings.emitter_count);

        /*
        //shader.setFloat("u_rho0", m_settings.);
        uniform float u_rho0 = 1.0;//g/mm3
        uniform float u_mass = 1.0;//g

        uniform float u_dt = 0.005;//s
        uniform float u_g = 9.81f * 1000.0;//mm/s^-2

        uniform float u_artificialPressureMultiplier = 0.0001;
        uniform float u_artificialViscosityMultiplier = 0.0001;
        uniform float u_viscosity = 0.0001;
        */

        /*
        particleShader->use();
        settings.numParticles.sync(*particleShader);
        settings.restDensity.sync(*particleShader);

        prefixSum->use();
        prefixSum->setUInt("dataSize", settings.bThread); //data size
        prefixSum->setUInt("blockSize", settings.blockSize); //block size

        isoGen->use();
        settings.numParticles.sync(*isoGen);
        settings.particleMass.sync(*isoGen);

        texPlot->use();
        settings.numParticles.sync(*texPlot);
        settings.particleMass.sync(*texPlot);
        */

    }
}