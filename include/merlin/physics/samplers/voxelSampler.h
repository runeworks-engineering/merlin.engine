#pragma once
#pragma once
#include "merlin/physics/particleSampler.h"

namespace Merlin {

	class VoxelSampler : public ParticleSampler {
	public:
		VoxelSampler(float spacing, float thickness = 0);
		~VoxelSampler() {};

		std::vector<glm::vec3> sample(Mesh_Ptr mesh) override;

	private:
		float m_spacing;
		float m_thickness;
	};

}

