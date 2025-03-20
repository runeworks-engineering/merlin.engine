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

		//void voxelize(float size);
		//void voxelizeSurface(float size, float thickness);

        void setShader(shared<Shader> shader);
        void setMaterial(shared<MaterialBase> material);
        void setShader(std::string shaderName);
        void setMaterial(std::string materialName);

		inline bool hasShader() const { return m_shader != nullptr; }
		inline bool hasMaterial() const { return m_material != nullptr; }

        void computeBoundingBox();

		inline const shared<Shader> getShader() const { return m_shader; }
		inline const std::string& getShaderName() const { return m_shaderName; }
		inline const shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }

		inline bool hasBoundingBox() const { return m_hasBoundingBox; }
		inline BoundingBox getBoundingBox() const { return m_bbox; }

        static shared<Model> create(std::string name);
        static shared<Model> create(const std::string& name, const shared<Mesh>& mesh);
        static shared<Model> create(const std::string& name, const std::vector<shared<Mesh>>& mesh);

    private:
        std::string m_materialName = "default";
        std::string m_shaderName = "default";

        shared<MaterialBase> m_material = nullptr;
        shared<Shader> m_shader = nullptr;

        bool m_hasBoundingBox = false;
        BoundingBox m_bbox = { glm::vec3(), glm::vec3() };
    };

    typedef shared<Model> Model_Ptr;

}