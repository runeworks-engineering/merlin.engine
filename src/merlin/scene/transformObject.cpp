#include "pch.h"
#include "merlin/scene/transformObject.h"
#include "merlin/utils/primitives.h"

namespace Merlin {

    TransformObject::TransformObject(const std::string& name, float size) : TransformObject::RenderableObject(name) {
        x_axis = Primitives::createArrow(size, glm::vec3(0.7, 0.0, 0.0), 30);
        y_axis = Primitives::createArrow(size, glm::vec3(0.0, 0.7, 0.0), 30);
        z_axis = Primitives::createArrow(size, glm::vec3(0.0, 0.0, 0.7), 30);
        sphere = Primitives::createSphere(size/35, 30, 30);

        sphere->smoothNormals();

        shared<PhongMaterial> xMaterial = createShared<PhongMaterial>("xMaterial");
        shared<PhongMaterial> yMaterial = createShared<PhongMaterial>("yMaterial");
        shared<PhongMaterial> zMaterial = createShared<PhongMaterial>("zMaterial");

        sphere->setMaterial("white");
        sphere->setRenderMode(RenderMode::UNLIT);

        y_axis->rotate(glm::vec3(0,0, 3.1415926 / 2.0));
        z_axis->rotate(glm::vec3(0,- 3.1415926/2.0,0));
        
    }

    shared<TransformObject> TransformObject::create(std::string name, float size) {
        return std::make_shared<TransformObject>(name, size);
    }



}