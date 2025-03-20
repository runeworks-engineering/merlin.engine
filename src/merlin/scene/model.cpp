#include "pch.h"
#include "merlin/scene/model.h"

namespace Merlin {


    Model::Model(const std::string& name) : RenderableObject(name){
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const shared<Mesh>& mesh) : RenderableObject(name) {
        addChild(mesh);
        
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const std::vector<shared<Mesh>>& meshes) : RenderableObject(name) {
        //m_meshes = meshes;
		for (auto mesh : meshes) {
			addChild(mesh);
		}
        m_transform = glm::mat4(1.0f);
    }

    shared<Model> Model::create(std::string name) {
        return createShared<Model>(name);
    }

    shared<Model> Model::create(const std::string& name, const shared<Mesh>& mesh) {
        return createShared<Model>(name, mesh);
    }

    shared<Model> Model::create(const std::string& name, const std::vector<shared<Mesh>>& meshes) {
        return createShared<Model>(name, meshes);
    }


    void Model::addMesh(const shared<Mesh>& mesh) {
        //m_meshes.push_back(mesh);
        addChild(mesh);
    }

	void Model::setShader(shared<Shader> shader)
	{
		m_shader = shader;
		for (const auto& obj : m_children)
			if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
				mesh->setShader(shader);
			}
	}

	void Model::setShader(std::string shaderName)
	{
		m_shaderName = shaderName;
		for (const auto& obj : m_children)
			if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
				mesh->setShader(shaderName);
			}
	}


	void Model::setMaterial(shared<MaterialBase> material){
		m_material = material;
		for (const auto& obj : m_children)
			if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
				mesh->setMaterial(material);
			}

	}

	

	void Model::setMaterial(std::string materialName){
		m_materialName = materialName;
		for (const auto& obj : m_children)
			if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
				mesh->setMaterial(materialName);
			}
	}

	void Model::computeBoundingBox() {
		glm::mat4 modelMat = globalTransform();
		glm::vec3 min(FLT_MAX), max(-FLT_MAX);

		for (const auto& obj : m_children)
		if (const auto mesh = std::dynamic_pointer_cast<Mesh>(obj)) {
			if (!mesh->hasBoundingBox()) mesh->computeBoundingBox();

			BoundingBox aabb = mesh->getBoundingBox();
			min = glm::min(min, aabb.min);
			max = glm::max(max, aabb.min);
		}

		glm::vec3 bbsize = max - min;

		if (bbsize.x * bbsize.y * bbsize.z == 0) {
			min -= glm::vec3(0.5);
			max += glm::vec3(0.5);
		}

		m_bbox.min = min;
		m_bbox.max = max;
		m_bbox.centroid = (min + max) / 2.0f;
		m_bbox.size = glm::abs(m_bbox.max - m_bbox.min);
		m_hasBoundingBox = true;
		Console::info("Model") << "Bounding box is " << m_bbox.max - m_bbox.min << " starting at " << m_bbox.min << " and ending at " << m_bbox.max << Console::endl;
	}


}