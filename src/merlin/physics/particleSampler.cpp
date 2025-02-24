#include "pch.h"
#include "merlin/physics/particleSampler.h"

namespace Merlin {

    ParticleSampler::ParticleSampler() {
    }

    ParticleSampler::~ParticleSampler() {
    }

    std::vector<glm::vec4> ParticleSampler::sample(Mesh_Ptr mesh) {
        return std::vector<glm::vec4>();
    }

    bool ParticleSampler::hasVolume() const{
        return true;
    }


}