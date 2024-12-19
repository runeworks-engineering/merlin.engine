#pragma once
#include "basePhysicsEntity.h"

namespace Merlin {
    class SoftBody : public BasePhysicsEntity {
    public:
        SoftBody() : BasePhysicsEntity();

        void stepPhysics(float dt) override {};

    private:

    };

}