#pragma once
#include "merlin/physics/physicsModifier.h"

namespace Merlin {
    class Emitter : public PhysicsModifier {
    public :
        Emitter(int count, float delay);

        float lastSpawnTime() const;
        float emitterDelay() const;
        GLuint count() const;
        GLuint phase() const;

        void setLastSpawnTime(float);
        void setEmitterDelay(float);
        void setPhase(GLuint);

    private:
        float m_emitterDelay = 0;
        float m_lastSpawnTime = 0;
        GLuint m_count = 0;
        GLuint m_phase = 0;
    };
}
