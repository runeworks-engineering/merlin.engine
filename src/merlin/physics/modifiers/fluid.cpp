#include "pch.h"
#include "merlin/physics/modifiers/fluid.h"

namespace Merlin {

    FluidModifier::FluidModifier()
        : PhysicsModifier(PhysicsModifierType::FLUID),
        restDensity("restDensity", 1.0f),
        particleMass("particleMass", 0.28f),
        artificialViscosityMultiplier("artificialViscosityMultiplier", 0.01f),
        artificialPressureMultiplier("artificialPressureMultiplier", 2780000.0f)
    {
    }

    void FluidModifier::onRenderMenu() {
            //Tree node version

        ImGui::SliderFloat("Rest Density", &restDensity.value(), 0.0f, 10.0f);
        ImGui::SliderFloat("Particle Mass", &particleMass.value(), 0.0f, 1.0f);
        ImGui::SliderFloat("Artificial Viscosity Multiplier (XSPH)", &artificialViscosityMultiplier.value(), 0.0f, 1.0f);
        ImGui::SliderFloat("Artificial Pressure Multiplier", &artificialPressureMultiplier.value(), 0.0f, 10000000.0f);
    }

}