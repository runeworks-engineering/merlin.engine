#include "pch.h"
#include "merlin/physics/modifiers/emitter.h"

namespace Merlin {
    Emitter::Emitter(int count, float delay) : 
        PhysicsModifier(PhysicsModifierType::EMITTER),  
        m_count(count), m_emitterDelay(delay) {

    }

    float Emitter::lastSpawnTime() const {
        return m_lastSpawnTime;
    }

    float Emitter::emitterDelay() const {
        return m_emitterDelay;
    }

    GLuint Emitter::count() const {
        return m_count;
    }

    GLuint Emitter::phase() const {
        return m_phase;
    }

    void Emitter::setPhase(GLuint phase)  {
        m_phase = phase;
    }

    void Emitter::setLastSpawnTime(float lastSpawnTime) {
        m_lastSpawnTime = lastSpawnTime;
    }

    void Emitter::setEmitterDelay(float delay) {
        m_emitterDelay = delay;
    }

}