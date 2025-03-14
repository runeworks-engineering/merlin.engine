#include "pch.h"
#include "merlin/scene/transformObject.h"
#include "merlin/utils/primitives.h"

namespace Merlin {

    TransformObject::TransformObject(const std::string& name, float size) : TransformObject::RenderableObject(name) {
        Mesh_Ptr axis_cyl;
        Mesh_Ptr axis_cone;

        axis_cyl = Primitives::createCylinder(size/40, size, 10);
        axis_cone = Primitives::createCone(size / 20, size / 20, 20);
        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>()*0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        x_axis = Model::create("x_axis", { axis_cyl , axis_cone });

        axis_cyl = Primitives::createCylinder(size / 40, size, 10);
        axis_cone = Primitives::createCone(size / 20, size / 20, 20);
        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>() * 0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        y_axis = Model::create("y_axis", { axis_cyl , axis_cone });

        axis_cyl = Primitives::createCylinder(size / 40, size, 10);
        axis_cone = Primitives::createCone(size / 20, size / 20, 20);
        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>() * 0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        z_axis = Model::create("z_axis", { axis_cyl , axis_cone });


        /*
        x_axis->calculateNormals();
        y_axis->calculateNormals();
        z_axis->calculateNormals();
        */

        shared<PhongMaterial> xMaterial = createShared<PhongMaterial>("xMaterial");
        shared<PhongMaterial> yMaterial = createShared<PhongMaterial>("yMaterial");
        shared<PhongMaterial> zMaterial = createShared<PhongMaterial>("zMaterial");

        zMaterial->setAmbient(glm::vec3(0.0, 0.00, 0.7));
        zMaterial->setDiffuse(glm::vec3(0.3, 0.3, 0.5));
        zMaterial->setSpecular(glm::vec3(0.04, 0.04, 0.7));
        zMaterial->setShininess(0.078125);

        yMaterial->setAmbient(glm::vec3(0.0, 0.7, 0.0));
        yMaterial->setDiffuse(glm::vec3(0.4, 0.5, 0.4));
        yMaterial->setSpecular(glm::vec3(0.04, 0.7, 0.04));
        yMaterial->setShininess(0.078125);

        xMaterial->setAmbient(glm::vec3(0.7, 0.0, 0.0));
        xMaterial->setDiffuse(glm::vec3(0.5, 0.4, 0.4));
        xMaterial->setSpecular(glm::vec3(0.7, 0.04, 0.04));
        xMaterial->setShininess(0.078125);

        x_axis->setMaterial(xMaterial);
        y_axis->setMaterial(yMaterial);
        z_axis->setMaterial(zMaterial);

        /*
        x_axis->setShader("model");
        y_axis->setShader("model");
        z_axis->setShader("model");
        */
        y_axis->rotate(glm::vec3(-3.1415926/2.0,0,0));
        x_axis->rotate(glm::vec3(0,3.1415926/2.0,0));

    }

    shared<TransformObject> TransformObject::create(std::string name, float size) {
        return std::make_shared<TransformObject>(name, size);
    }



}