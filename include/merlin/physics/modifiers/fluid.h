#pragma once
#include "merlin/physics/physicsModifier.h"
#include "merlin/shaders/uniform.h"
#include "merlin/physics/physicsEnum.h"

namespace Merlin {
    class FluidModifier : public PhysicsModifier {
    public:
        FluidModifier();

        void onRenderMenu() override;

    private:
        Uniform<float> restDensity;
        Uniform<float> particleMass;
        Uniform<float> artificialViscosityMultiplier;//XSPH
        Uniform<float> artificialPressureMultiplier;

        bool usePositivePressureOnly = false;
        PressureSolver pressureSolver;
        ViscositySolver viscositySolver;
    };
}