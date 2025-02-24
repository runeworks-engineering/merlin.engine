#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/mesh.h"

namespace Merlin {

    // ParticleSampler class with multiple sampling methods.
    class ParticleSampler {
    public:
        ParticleSampler();
        ~ParticleSampler();

        virtual std::vector<glm::vec4> sample(Mesh_Ptr mesh);
        virtual bool hasVolume() const;

    private:
    };

    typedef shared<ParticleSampler> ParticleSampler_Ptr;
} // namespace Merlin