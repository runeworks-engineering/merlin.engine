#pragma once
#include "merlin/core/core.h"
#include "merlin/graphics/mesh.h"
#include "merlin/graphics/material.h"
#include "merlin/scene/camera.h"
#include "merlin/graphics/renderableObject.h"

#include <map>

namespace Merlin {

    class Model : public RenderableObject {
    public:

        Model(const std::string& name);
        Model(const std::string& name, const shared<Mesh>& mesh);
        Model(const std::string& name, const std::vector<shared<Mesh>>& mesh);

        void addMesh(const shared<Mesh>& mesh);
        void draw(const Camera& camera) const;

        void enableWireFrameMode() override;
        void disableWireFrameMode() override;


        void setShader(const shared<Shader>& shader);
        void setMaterial(shared<MaterialBase> material);
        void setShader(const std::string& shader);
        void setMaterial(const std::string& material);

        inline const std::vector<shared<Mesh>>& meshes() const { return m_meshes; }

        static shared<Model> create(std::string name);
        static shared<Model> create(const std::string& name, const shared<Mesh>& mesh);
        static shared<Model> create(const std::string& name, const std::vector<shared<Mesh>>& mesh);



    protected:
        std::vector<shared<Mesh>> m_meshes;
        //TODO : Group Meshes by Shaders and Material to optimize draw call
    };

    typedef shared<Model> Model_Ptr;

}