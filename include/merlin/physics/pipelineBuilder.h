#pragma once
#include "merlin/core/core.h"
#include "merlin/physics/physicsEnum.h"
#include "merlin/physics/physicsPipeline.h"

namespace Merlin {
    class CustomPipelineBuilder {
    public:
        CustomPipelineBuilder(PhysicsPipeline_Ptr pipeline);

        CustomPipelineBuilder& step(const std::string& name);
        CustomPipelineBuilder& usesShader(const std::string& shaderPath);
        CustomPipelineBuilder& activates(std::initializer_list<PhysicsFlag> tags);
        void done();

    private:
        PhysicsPipeline_Ptr m_pipeline;
        PhysicsPipelineStep_Ptr m_currentStep;
    };
}