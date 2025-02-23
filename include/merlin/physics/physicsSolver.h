#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/particleSystem.h"

namespace Merlin {
	struct PhysicsSettings {
		// --- physics settings ---
		float particle_radius = 1.0;
		float smoothingRadius = 3 * particleRadius;
		float cell_width = smoothingRadius;

		Uniform<float> timestep = Uniform<float>("timestep", 1.0f / 60.0f);
		Uniform<float> particlem_mass = Uniform<float>("particlem_mass", 1.0);

		GLuint particles_count = 1;
		GLuint max_particles_count = 2000000; //Max Number of particles (10 milion)

		// --- solver settings ---
		int solver_substep = 4;
		int solver_iteration = 3;
		float overRelaxation = 1.0; 

		// --- threading settings ---
		GLuint pWkgSize = 1024; //Number of thread per workgroup (particles)
		GLuint bWkgSize = 512;  //Number of thread per workgroup (cells)
	};


	class PhysicsSolver {
	public:
		PhysicsSolver();
		~PhysicsSolver() {};

		void init();
		void clean();
		void setSettings(PhysicsSettings);

		void update(Timestep ts);
		void add(PhysicsEntity_Ptr);
		void setDomain(BoundingBox);

		bool useIndexSorting();
		bool useSparseBuffer();
		void useIndexSorting(bool);
		void useSparseBuffer(bool);

		
	private:
		void warmUnstagedChanges();
		void errorSolverNotReady();

	private:
		bool m_active = true;
		bool m_ready = false;

		// --- algorithm settings ---
		bool use_index_sorting = false; //sort the particle instead
		bool use_sparse_buffer = false; //smaller footprint, but index_sorting becomes mandatory.

		std::vector<PhysicsEntity_Ptr> m_entity;

		ParticleSystem_Ptr m_particles = nullptr;
		ParticleSystem_Ptr m_bins = nullptr;

		PhysicsSettings m_settings;
		BoundingBox m_domain;
	};
}



