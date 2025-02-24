#pragma once
#include "merlin/core/core.h"
#include "merlin/core/timestep.h"
#include "merlin/Physics/physicsEntity.h"
#include "merlin/Physics/nns.h"

namespace Merlin {


	struct PhysicsSettings {
		// --- physics settings ---
		float particle_radius = 1.0;
		float smoothing_radius = 3 * particle_radius;
		float cell_width = smoothing_radius;

		Uniform<float> timestep = Uniform<float>("timestep", 1.0f / 60.0f);
		Uniform<float> particlem_mass = Uniform<float>("particlem_mass", 1.0);

		GLuint particles_count = 1;
		GLuint max_particles_count = 2000000; //Max Number of particles (10 milion)

		// --- solver settings ---
		int solver_substep = 4;
		int solver_iteration = 3;
		float overRelaxation = 1.0; 

		PressureSolver fluid_solver = PressureSolver::PBF;
		ViscositySolver visco_solver = ViscositySolver::XSPH;
		SoftBodySolver soft_solver = SoftBodySolver::PBD_WDC;
		RigidBodySolver rigid_solver = RigidBodySolver::SHAPE_MATCHING;
		GranularBodySolver granular_solver = GranularBodySolver::PBD_DC;

		// --- threading settings ---
		GLuint pWkgSize = 1024; //Number of thread per workgroup (particles)
	};

	class PhysicsSolver {
	public:
		PhysicsSolver();
		~PhysicsSolver() {};

		void init();
		void clean();

		void reset();
		void update(Timestep ts);
		void add(PhysicsEntity_Ptr);
		void spawnParticles(int count, int phase);


		void setDomain(BoundingBox);
		void setTimestep(float dt); //ms
		void setSettings(PhysicsSettings);
		
		BoundingBox getDomain();
		float getTimestep();
		PhysicsSettings& getSettings();

		void addPhysics(PhysicsModifierType);
		bool hasPhysics(PhysicsModifierType);

		bool useIndexSorting();
		bool useSparseBuffer();
		bool useDynamicBuffer();
		void useIndexSorting(bool);
		void useSparseBuffer(bool);
		
	private:
		void warmUnstagedChanges();
		void errorSolverNotReady();
		void setConstants(ShaderBase& shader);
		void setUniforms(ShaderBase& shader);
		void computeThreadLayout();


		void generateFields();
		void add_PBD_Buffers();
		void add_MMC_Buffers();

	private:
		
		std::vector<glm::vec4> 
			cpu_position_buffer,
			cpu_last_position_buffer,
			cpu_predicted_position_buffer,
			cpu_correction_position_buffer,
			cpu_velocity_buffer,
			cpu_emitter_position_buffer;

		std::vector<float> 
			cpu_density_buffer,
			cpu_lambda_buffer,
			cpu_dlambda_buffer,
			cpu_temperature_buffer;

		std::vector<float[8]> cpu_stress_buffer;
		std::vector<glm::uvec4> cpu_meta_buffer;
		

	private:
		bool m_active = true;
		bool m_ready = false;

		// --- algorithm settings ---
		bool use_index_sorting = false; //sort the particle instead
		bool use_sparse_buffer = false; //smaller footprint, but index_sorting becomes mandatory.
		bool use_dynamic_buffer = false; //Used by particle emitters (disabled otherwise)

	private:
		std::vector<PhysicsEntity_Ptr> m_entity;
		std::vector<PhysicsEntity_Ptr> m_active_entities;
		std::vector<PhysicsEntity_Ptr> m_active_emitters;

		std::set<PhysicsModifierType> m_active_physics;

		ParticleSystem_Ptr m_particles = nullptr;
		StagedComputeShader_Ptr m_solver;
		
		NNS_Ptr m_grid;
		NNS_Ptr m_static_grid;

		PhysicsSettings m_settings;
		BoundingBox m_domain;

		//GPU Threading settings
		GLuint m_pWkgCount; //Total number of workgroup needed
		float m_elapsed_time = 0;
	};
}



