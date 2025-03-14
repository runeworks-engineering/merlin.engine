#pragma once
#include "merlin/core/core.h"
#include "merlin/scene/model.h"
#include "merlin/graphics/material.h"
#include "merlin/scene/camera.h"
#include "merlin/graphics/renderableObject.h"


#include <map>

namespace Merlin {

    class TransformObject : public RenderableObject {
    public:

        TransformObject(const std::string& name, float size = 1);
        static shared<TransformObject> create(std::string name, float size = 1);

        inline const Model_Ptr& getXAxisModel() const { return x_axis; }
        inline const Model_Ptr& getYAxisModel() const { return y_axis; }
        inline const Model_Ptr& getZAxisModel() const { return z_axis; }
        inline const Mesh_Ptr& getSphereMesh() const { return sphere; }

    private :
        Model_Ptr x_axis;
        Model_Ptr y_axis;
        Model_Ptr z_axis;
        Mesh_Ptr sphere;
    };

    typedef shared<TransformObject> TransformObject_Ptr;

}