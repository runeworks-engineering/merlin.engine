#pragma once
#include "merlin/core/core.h"
#include "merlin/core/timestep.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/physics/samplers/voxelSampler.h"
#include "merlin/physics/nns.h"
#include "merlin/physics/physicsPipeline.h"

#include <set>
#include <unordered_map>

namespace Merlin {

	/* Fields
        m_particles->addField<glm::vec4>("position_buffer", true);
        m_particles->addField<glm::vec4>("velocity_buffer", true);
        m_particles->addField<glm::uvec4>("meta_buffer", true);
			#define phase(index)		ssbo_meta[index].x		//phase (fluid, liquid...)
			#define entity(index)		ssbo_meta[index].y		//entity ID
			#define id(index)			ssbo_meta[index].z		//particle ID
			#define sorted_id(index)	ssbo_meta[index].w		//particle destination ID (sort)

        m_particles->addField<float>("density_buffer", true);
        m_particles->addField<float>("pressure_buffer", false);
        m_particles->addField<glm::vec4>("rest_position_buffer");
        m_particles->addField<glm::mat4>("plasticity_buffer");
        m_particles->addField<float[2]>("temperature_buffer");
			// T, dT
		m_particles->addField<float[2]>("lambda_buffer"); 
			//dlambda, dlambda

		m_particles->addField<glm::vec4>("predicted_position_buffer");
		m_particles->addField<glm::vec4>("position_correction_buffer");

		m_particles->addField<glm::mat4>("F_buffer");
		m_particles->addField<glm::mat4>("L_buffer");
        m_particles->addField<glm::mat3x4>("stress_buffer");
	*/

	struct PhysicsSettings3D {
		// --- physics settings ---
		ConstantUniform<float> particle_radius = 
			ConstantUniform<float>("cst_particleRadius", 1.0);

		ConstantUniform<float> smoothing_radius = 
			ConstantUniform<float>("cst_smoothingRadius", 3 * particle_radius.value());

		ConstantUniform<float> cell_width = 
			ConstantUniform<float>("cst_binSize", smoothing_radius.value());

		Uniform<float> timestep = Uniform<float>("u_dt", 1.0f / 60.0f);
		Uniform<float> particle_mass = Uniform<float>("u_mass", 1.0);

		GLuint initial_particles_count = 1;
		Uniform <GLuint> particles_count = Uniform<GLuint>("u_numParticles", 1);
		Uniform <GLuint> max_particles_count = Uniform<GLuint>("u_maxParticles", 2000000);
		Uniform <GLuint> emitter_count = Uniform<GLuint>("u_numEmitter", 0);

		// --- solver settings ---
		int solver_substep = 4;
		int solver_iteration = 3;
		float overRelaxation = 1.0; 

		bool usePositivePressureOnly = false;
		PressureSolver pressureSolver = PressureSolver::PBF;
		ViscositySolver viscositySolver = ViscositySolver::XSPH;
		SoftBodySolver softBodySolver = SoftBodySolver::PBD_WDC;
		RigidBodySolver rigidBodySolver = RigidBodySolver::SHAPE_MATCHING;
		GranularBodySolver granularBodySolver = GranularBodySolver::PBD_DC;

		// --- threading settings ---
		GLuint pWkgSize = 1024; //Number of thread per workgroup (particles)
	};

	class PhysicsSolver3D {
	public:
		PhysicsSolver3D();
		~PhysicsSolver3D() {};

		void init();
		void clean();

		void reset();
		void update(Timestep ts);
		void add(PhysicsEntity_Ptr);
		void spawnParticles(int count, int phase);

		void attachGraphics();
		void detachGraphics();

		ParticleSystem_Ptr getParticles();
		ParticleSystem_Ptr getBins();

		std::vector<PhysicsEntity_Ptr> getEntities();

		void setDomain(BoundingBox);
		void setTimestep(float dt); //ms
		void setSettings(PhysicsSettings3D);
		
		BoundingBox getDomain();
		float getTimestep();
		PhysicsSettings3D& getSettings();

		void addPhysics(PhysicsModifierType);
		bool hasPhysics(PhysicsModifierType);

		bool useIndexSorting() const;
		bool useSparseBuffer() const;
		bool useDynamicBuffer() const;
		void useIndexSorting(bool);
		void useSparseBuffer(bool);
		void useFixedTimeStep(bool);
		
		void scanScene();

		float getTime() const;

		void onRenderMenu();

	private:
		void warmUnstagedChanges();
		void errorSolverNotReady();
		void setConstants(ShaderBase& shader);
		void setUniforms(ShaderBase& shader);
		void computeThreadLayout();

		void gatherParticleGroup();

		//void generateBuffer();
		//void uploadFields();
		//void generateFields();
		void generateDefaultPipeline();

		//void add_PBD_Buffers();
		//void add_MMC_Buffers();

	private:
		bool m_active = true;
		bool m_ready = false;


		// --- default buffers ---
		std::vector<glm::vec4> cpu_position_buffer;
		std::vector<glm::vec4> cpu_emitter_position_buffer;
		std::vector<glm::uvec4> cpu_meta_buffer;


		// --- algorithm settings ---
		bool use_index_sorting = false; //sort the particle instead
		bool use_sparse_buffer = false; //smaller footprint, but index_sorting becomes mandatory.
		bool use_dynamic_buffer = false; //Used by particle emitters (disabled otherwise)
		bool use_fixed_timestep = true; //Used by particle emitters (disabled otherwise)

	private:
		std::vector<PhysicsEntity_Ptr> m_entity;
		std::unordered_map<std::string, PhysicsEntity_Ptr> m_active_entities;
		std::unordered_map<std::string, PhysicsEntity_Ptr> m_active_emitters;
		std::set<PhysicsModifierType> m_active_physics;
		
		Shader_Ptr pshader = nullptr;
		Shader_Ptr bshader = nullptr;

		ParticleSystem_Ptr m_particles = nullptr;
		PhysicsPipeline_Ptr m_pipeline = nullptr;


		ComputeShader_Ptr m_filter;
		
		NNS_Ptr m_grid;
		NNS_Ptr m_static_grid;

		PhysicsSettings3D m_settings;
		BoundingBox m_domain;

		//GPU Threading settings
		GLuint m_pWkgCount; //Total number of workgroup needed
		float m_elapsed_time = 0;
	};
}



