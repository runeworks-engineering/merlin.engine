#include "pch.h"
#include "merlin/scene/model.h"

namespace Merlin {


    Model::Model(const std::string& name) : RenderableObject(name){
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const shared<Mesh>& mesh) : RenderableObject(name) {
        m_meshes.push_back(mesh);
        addChild(mesh);
        
        m_transform = glm::mat4(1.0f);
    }

    Model::Model(const std::string& name, const std::vector<shared<Mesh>>& meshes) : RenderableObject(name) {
        m_meshes = meshes;
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
        m_meshes.push_back(mesh);
        addChild(mesh);
    }

	void Model::setShader(const shared<Shader>& shader) {
		for (auto& mesh : m_meshes) {
			mesh->setShader(shader);
		}
	}

	void Model::setMaterial(shared<MaterialBase> material) {
		for (auto& mesh : m_meshes) {
			mesh->setMaterial(material);
		}
	}

	void Model::setShader(const std::string& shader) {
		for (auto& mesh : m_meshes) {
			mesh->setShader(shader);
		}
	}

	void Model::setMaterial(const std::string& material) {
		for (auto& mesh : m_meshes) {
			mesh->setMaterial(material);
		}
	}


    void Model::draw(const Camera& camera) const {/*
        for (const auto& mesh : m_meshes) {
            if (mesh->hasMaterial() && mesh->hasShader()) {
				shared<Shader> shader = mesh->getShader();
				shared<MaterialBase> mat = mesh->getMaterial();

                shader->use();
                mat->attach(*shader);

				mesh->draw();
            }
        }*/
    }

    void Model::enableWireFrameMode(){
        for (auto mesh : m_meshes) {
            mesh->enableWireFrameMode();
        }
        RenderableObject::enableWireFrameMode();
    }

    void Model::disableWireFrameMode(){
        for (auto mesh : m_meshes) {
            mesh->disableWireFrameMode();
        }
        RenderableObject::disableWireFrameMode();
    }



}