#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/physicsEntity.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/physicsModifier.h"
#include "merlin/physics/particleSystem.h"

namespace Merlin {
	class PhysicsSolver {
	public:
		PhysicsSolver();
		~PhysicsSolver() {};

		void init();
		void clean();
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

		//settings
		bool use_index_sorting = false; //sort the particle instead
		bool use_sparse_buffer = false; //smaller footprint, but index_sorting becomes mandatory.

		std::vector<PhysicsEntity_Ptr> m_entity;

		ParticleSystem_Ptr m_particles = nullptr;
		ParticleSystem_Ptr m_bins = nullptr;

		BoundingBox m_domain;
	};
}



