#include "pch.h"
#include "merlin/scene/transformObject.h"
#include "merlin/utils/primitives.h"

namespace Merlin {

    TransformObject::TransformObject(const std::string& name, float size) : TransformObject::RenderableObject(name) {
        Mesh_Ptr axis_cyl;
        Mesh_Ptr axis_cone;


		float cone_height = size / 10;
		float cone_radius = size / 25;
		float cylinder_radius = size / 40;
		float cylinder_height = size;

        axis_cyl = Primitives::createCylinder(cylinder_radius, cylinder_height, 30);
        axis_cone = Primitives::createCone(cone_radius, cone_height, 30);
        sphere = Primitives::createSphere(cone_radius*1.2, 20, 20);

        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>()*0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        x_axis = Model::create("x_axis", { axis_cyl , axis_cone });

        axis_cyl = Primitives::createCylinder(cylinder_radius, cylinder_height, 30);
        axis_cone = Primitives::createCone(cone_radius, cone_height, 30);
        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>() * 0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        y_axis = Model::create("y_axis", { axis_cyl , axis_cone });

        axis_cyl = Primitives::createCylinder(cylinder_radius, cylinder_height, 30);
        axis_cone = Primitives::createCone(cone_radius, cone_height, 30);
        axis_cone->translate(glm::vec3(size, 0, 0));
        //axis_cone->rotate(glm::vec3(0, 0, -glm::pi<float>() * 0.5));
        //axis_cone->applyMeshTransform();
        axis_cone->computeNormals();
        axis_cone->smoothNormals();

        z_axis = Model::create("z_axis", { axis_cyl , axis_cone });

        sphere->smoothNormals();

        shared<PhongMaterial> xMaterial = createShared<PhongMaterial>("xMaterial");
        shared<PhongMaterial> yMaterial = createShared<PhongMaterial>("yMaterial");
        shared<PhongMaterial> zMaterial = createShared<PhongMaterial>("zMaterial");

        zMaterial->setAmbient(glm::vec3(0.0, 0.0, 0.7));
        yMaterial->setAmbient(glm::vec3(0.0, 0.7, 0.0));
        xMaterial->setAmbient(glm::vec3(0.7, 0.0, 0.0));


        x_axis->setMaterial(xMaterial);
        y_axis->setMaterial(yMaterial);
        z_axis->setMaterial(zMaterial);
        sphere->setMaterial("defaultWhite");

        x_axis->setRenderMode(RenderMode::UNLIT);
        y_axis->setRenderMode(RenderMode::UNLIT);
        z_axis->setRenderMode(RenderMode::UNLIT);
        sphere->setRenderMode(RenderMode::UNLIT);

        /*
        x_axis->setShader("debug.normals");
        y_axis->setShader("debug.normals");
        z_axis->setShader("debug.normals");
        /**/

        y_axis->rotate(glm::vec3(0,0, 3.1415926 / 2.0));
        z_axis->rotate(glm::vec3(0,- 3.1415926/2.0,0));
        
    }

    shared<TransformObject> TransformObject::create(std::string name, float size) {
        return std::make_shared<TransformObject>(name, size);
    }



}