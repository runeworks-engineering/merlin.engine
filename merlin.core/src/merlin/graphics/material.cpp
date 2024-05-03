#include "glpch.h"
#include "merlin/graphics/material.h"
#include "merlin/shaders/shader.h"
#include <glm/glm.hpp>

namespace Merlin {
	Material::Material(std::string name) : m_name(name) { loadDefaultTexture(); }

    void Material::setProperty(const MaterialProperty& props) {
		m_props = props;
    }

    void Material::setAmbient(const glm::vec3& ambient) { m_props.ambient = ambient;}
	
    void Material::setDiffuse(const glm::vec3& diffuse) { m_props.diffuse = diffuse; }

    void Material::setSpecular(const glm::vec3& specular) { m_props.specular = specular; }

    void Material::setShininess(const float& shininess) { m_props.shininess = shininess; }

	void Material::setAlbedo(const glm::vec3& albedo) { m_props.albedo = albedo; }

	void Material::setMetallic(const float& metallic) { m_props.metallic = metallic; }

	void Material::setRoughness(const float& roughness) { m_props.roughness = roughness; }

	void Material::setAmbientOcclusion(const float& ao) { m_props.ao = ao; }

	void Material::setTexture(Texture_Ptr tex) {
		m_textures[tex->type()] = tex;
	}

	void Material::loadTexture(std::string path, TextureType t) {
		Shared<Texture> tex = createShared<Texture>(t);
		tex->bind();
		tex->loadFromFile(path);
		tex->setInterpolationMode(GL_LINEAR);
		tex->setRepeatMode(GL_REPEAT);
		setTexture(tex);
	}

	Shared<Texture> GenerateDefaultTexture(TextureType t) {
		// Determine the default color for the texture type
		glm::vec3 defaultColor;
		switch (t) {
		case TextureType::COLOR:
			defaultColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			break;
		case TextureType::ROUGHNESS:
			defaultColor = glm::vec3(0.04f, 0.04f, 0.04f); // Dark gray
			break;
		case TextureType::NORMAL:
			defaultColor = glm::vec3(0.5f, 0.5f, 1.0f); // Normal map default (x: 0.5, y: 0.5, z: 1.0)
			break;
		case TextureType::AMBIENT_OCCLUSION:
			defaultColor = glm::vec3(1.0f, 1.0f, 1.0f); // White
			break;
		case TextureType::EMISSION:
			defaultColor = glm::vec3(0.0f, 0.0f, 0.0f); // Black
			break;
			// Add more cases as needed
		default:
			defaultColor = glm::vec3(0.5f, 0.5f, 0.5f); // Gray
			break;
		}

		// create a 1x1 texture with the default color
		Shared<Texture> defaultTexture = std::make_shared<Texture>(t); // Adapt this line to your Texture class
		defaultTexture->bind();
		defaultTexture->reserve(1, 1, GL_RGB);
		defaultTexture->loadFromData((unsigned char*)&defaultColor[0], 1, 1, GL_RGB);
		defaultTexture->setInterpolationMode(GL_LINEAR, GL_LINEAR);
		defaultTexture->setRepeatMode(GL_REPEAT, GL_REPEAT);
		defaultTexture->generateMipMap();

		return defaultTexture;
	}

	void Material::loadDefaultTexture() {
		m_textures[TextureType::COLOR] = GenerateDefaultTexture(TextureType::COLOR);
		m_textures[TextureType::NORMAL] = GenerateDefaultTexture(TextureType::NORMAL);
		m_textures[TextureType::ROUGHNESS] = GenerateDefaultTexture(TextureType::ROUGHNESS);
		m_textures[TextureType::AMBIENT_OCCLUSION] = GenerateDefaultTexture(TextureType::AMBIENT_OCCLUSION);
		m_textures[TextureType::EMISSION] = GenerateDefaultTexture(TextureType::EMISSION);
	}

	Texture& Material::getTexture(TextureType t) const {
		if (m_textures.find(t) != m_textures.end()) return *m_textures.at(t);
		else return *m_textures.at(TextureType::COLOR);
	}




}