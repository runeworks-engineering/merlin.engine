#include "pch.h"
#include "merlin/physics/modifiers/fluid.h"

namespace Merlin {

    FluidModifier::FluidModifier()
        : PhysicsModifier(PhysicsModifierType::FLUID),
        restDensity("restDensity", 1.0f),
        particleMass("particleMass", 0.28f),
        artificialViscosityMultiplier("artificialViscosityMultiplier", 0.01f),
        artificialPressureMultiplier("artificialPressureMultiplier", 2780000.0f),
        usePositivePressureOnly(false),
        pressureSolver(PressureSolver::PBF),
        viscositySolver(ViscositySolver::XSPH)
    {
    }

    void FluidModifier::onRenderMenu() {
            //Tree node version

        ImGui::SliderFloat("Rest Density", &restDensity.value(), 0.0f, 10.0f);
        ImGui::SliderFloat("Particle Mass", &particleMass.value(), 0.0f, 1.0f);
        ImGui::SliderFloat("Artificial Viscosity Multiplier (XSPH)", &artificialViscosityMultiplier.value(), 0.0f, 1.0f);
        ImGui::SliderFloat("Artificial Pressure Multiplier", &artificialPressureMultiplier.value(), 0.0f, 10000000.0f);
        ImGui::Checkbox("Use Positive Pressure Only", &usePositivePressureOnly);

        const char* pressureSolvers[] = { "WSPH", "PBF" };
        static int pressureSolverIndex = 0;
        if (ImGui::Combo("Pressure Solver", &pressureSolverIndex, pressureSolvers, IM_ARRAYSIZE(pressureSolvers))) {
            pressureSolver = (PressureSolver)pressureSolverIndex;
        }

        const char* viscositySolvers[] = { "XSPH" };
        static int viscositySolverIndex = 0;
        if (ImGui::Combo("Viscosity Solver", &viscositySolverIndex, viscositySolvers, IM_ARRAYSIZE(viscositySolvers))) {
            viscositySolver = (ViscositySolver)viscositySolverIndex;
        }
    }

}