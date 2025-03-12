#include "pch.h"
#include "merlin/scene/light.h"
#include "merlin/core/core.h"

namespace Merlin {

	void Light::draw() const {
		m_mesh->draw();
	}

	void Light::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
	}

	void DirectionalLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setInt(base + ".castShadow", m_castShadow);
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 0.0f)));
		if (m_shadowMap && m_castShadow) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);
			m_shadowMap->autoSetUnit();
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".shadowMap");
		}
	}

	void DirectionalLight::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
		if (m_shadowMap) {
			m_shadowMap->bind();
			m_shadowMap->resize(m_shadowResolution, m_shadowResolution);
			m_shadowMap->unbind();
		}
		if (m_shadowFBO) {
			m_shadowFBO->bind();
			m_shadowFBO->resize(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->unbind();
		}
	}

	void DirectionalLight::detach() {
		if (m_shadowMap) {
			m_shadowMap->unbind();
		}
	}

	void DirectionalLight::attachShadow(Shader& shader, const std::vector<glm::vec3>& points) {

		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		// Check if the light direction is collinear with the default up vector
		if (glm::abs(glm::dot(direction(), upVector)) > 0.999f)
			upVector = glm::vec3(1.0f, 0.0f, 0.0f); // Choose an alternative up vector

		glm::mat4 lightView = glm::lookAt(-direction(), glm::vec3(0.0f, 0.0f, 0.0f), upVector);

		// Déterminer les plans near et far
		float near_plane = std::numeric_limits<float>::max();
		float far_plane = std::numeric_limits<float>::lowest();
		float left = std::numeric_limits<float>::max();
		float right = std::numeric_limits<float>::lowest();
		float top = std::numeric_limits<float>::lowest();
		float bottom = std::numeric_limits<float>::max();

		for (const auto& wpoint : points) {
			const auto& point = glm::vec3(lightView * glm::vec4(wpoint, 1.0f));
			near_plane = std::min(near_plane, point.z);
			far_plane = std::max(far_plane, point.z);
			left = std::min(left, point.x);
			right = std::max(right, point.x);
			bottom = std::min(bottom, point.y);
			top = std::max(top, point.y);
		}

		// Ajuster les valeurs pour éviter les artefacts
		//near_plane = 1.0 * near_plane;
		//far_plane = 1.0 * far_plane;

		glm::mat4 lightProjection = glm::ortho(left, right, bottom, top, near_plane, far_plane);

		m_lightSpaceMatrix = lightProjection * lightView;
		mesh().setTransform(lightView);

		shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
	}

	void DirectionalLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = Texture2D::create(m_shadowResolution, m_shadowResolution, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}
	}

	void PointLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setInt(base + ".castShadow", m_castShadow);
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f)));

		if (m_shadowMap && m_castShadow) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);

			m_shadowMap->autoSetUnit();
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".omniShadowMap");
			shader.setFloat(base + ".far_plane", 25.0f);
		}
	}

	void PointLight::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
		if (m_shadowMap) {
			m_shadowMap->bind();
			m_shadowMap->resize(m_shadowResolution, m_shadowResolution);
			m_shadowMap->unbind();
		}
		if (m_shadowFBO) {
			m_shadowFBO->bind();
			m_shadowFBO->resize(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->unbind();
		}
	}

	void PointLight::detach() {
		if (m_shadowMap) {
			m_shadowMap->unbind();
		}
	}


	void PointLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = CubeMap::create(m_shadowResolution, m_shadowResolution, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}
	}


	glm::mat4 computeViewMAtrix(glm::vec3 position, glm::vec3 direction) {
		glm::vec3 _Front = normalize(direction);
		glm::vec3 _Right = glm::normalize(glm::cross(_Front, glm::vec3(0, 0, 1)));
		glm::vec3 _Up = glm::normalize(glm::cross(_Right, _Front));

		return glm::lookAt(position, position + _Front, _Up);
	}


	void PointLight::attachShadow(Shader& shader, const std::vector<glm::vec3>& points) {
		glm::vec3 pos = position();

		// Déterminer les plans near et far
		float near_plane = std::numeric_limits<float>::max();
		float far_plane = std::numeric_limits<float>::lowest();

		for (const auto& point : points) {
			near_plane = std::min(near_plane, glm::length(point - pos));
			far_plane = std::max(far_plane, glm::length(point - pos));
		}

		//Console::print() << "Near: " << near_plane << " Far: " << far_plane << Console::endl;
		// Ajuster les valeurs pour éviter les artefacts
		near_plane = 0.2 * std::max(near_plane, 0.1f);
		far_plane = std::max(far_plane, near_plane + 0.1f);

		m_lightSpaceMatrix = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		m_shadowTransforms.clear();
		// Correct orientations for each face of the cubemap
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // Positive X
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // Negative X
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))); // Positive Y
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))); // Negative Y
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0))); // Positive Z
		m_shadowTransforms.push_back(m_lightSpaceMatrix * glm::lookAt(pos, pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))); // Negative Z

		for (int i = 0; i < m_shadowTransforms.size(); i++) {
			shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", m_shadowTransforms[i]);
		}
		
		shader.setVec3("lightPos", pos);
		shader.setFloat("far_plane", far_plane);
	}


	void AmbientLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", glm::vec3(0));
		shader.setVec3(base + ".specular", glm::vec3(0));
		//shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		//shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(glm::vec3(0,0,1000000), 1.0f)));
	}

	void SpotLight::attach(int id, Shader& shader) {
		std::string base = "lights[" + std::to_string(id) + "]";
		shader.setVec3(base + ".ambient", ambient());
		shader.setVec3(base + ".diffuse", diffuse());
		shader.setVec3(base + ".specular", specular());
		shader.setVec3(base + ".attenuation", attenuation());
		shader.setInt(base + ".type", static_cast<int>(type()));
		shader.setVec3(base + ".position", glm::vec3(getRenderTransform() * glm::vec4(position(), 1.0f))); //point
		shader.setVec3(base + ".direction", glm::vec3(getRenderTransform() * glm::vec4(direction(), 0.0f))); //pure vector
		shader.setFloat(base + ".cutOff", cutOff());
		if (m_shadowMap && m_castShadow) {
			shader.setMat4(base + ".lightSpaceMatrix", m_lightSpaceMatrix);
			m_shadowMap->autoSetUnit();
			m_shadowMap->bind();
			m_shadowMap->syncTextureUnit(shader, base + ".shadowMap");
		}
	}

	void SpotLight::setShadowResolution(GLuint res) {
		m_shadowResolution = res;
		if (m_shadowMap) {
			m_shadowMap->bind();
			m_shadowMap->resize(m_shadowResolution, m_shadowResolution);
			m_shadowMap->unbind();
		}
		if (m_shadowFBO) {
			m_shadowFBO->bind();
			m_shadowFBO->resize(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->unbind();
		}
	}

	void SpotLight::detach() {
		if (m_shadowMap) {
			m_shadowMap->unbind();
		}
	}

	void SpotLight::attachShadow(Shader& shader, const std::vector<glm::vec3>& points) {
		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		// Check if the light direction is collinear with the default up vector
		if (glm::abs(glm::dot(direction(), upVector)) > 0.999f)
			upVector = glm::vec3(1.0f, 0.0f, 0.0f); // Choose an alternative up vector

		glm::mat4 lightView = glm::lookAt(position(), position() + direction(), upVector);

		// Déterminer les plans near et far
		float near_plane = std::numeric_limits<float>::max();
		float far_plane = std::numeric_limits<float>::lowest();

		for (const auto& wpoint : points) {
			const auto& point = glm::vec3(lightView * glm::vec4(wpoint, 1.0f));
			near_plane = std::min(near_plane, point.z);
			far_plane = std::max(far_plane, point.z);
		}

		// Ajuster les valeurs pour éviter les artefacts
		//near_plane = std::max(near_plane, 0.1f);
		//far_plane = std::max(far_plane, near_plane + 0.1f);

		glm::mat4 lightProjection = glm::perspective(glm::radians(cutOff()), 1.0f, near_plane, far_plane);

		m_lightSpaceMatrix = lightProjection * lightView;
		mesh().setTransform(lightView);

		shader.setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
	}




	void SpotLight::generateShadowMap() {
		if (!m_shadowMap) {
			m_shadowMap = Texture2D::create(m_shadowResolution, m_shadowResolution, TextureType::SHADOW);
			m_shadowMap->bind();
			m_shadowMap->setInterpolationMode(GL_LINEAR, GL_LINEAR);
			m_shadowMap->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
			m_shadowMap->setBorderColor4f(1, 1, 1, 1);
			m_shadowMap->unbind();
		}
		if (!m_shadowFBO) {
			m_shadowFBO = createShared<FBO>(m_shadowResolution, m_shadowResolution);
			m_shadowFBO->bind();
			m_shadowMap->bind();
			m_shadowFBO->attachDepthStencilTexture(m_shadowMap);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			m_shadowFBO->unbind();
			m_shadowMap->unbind();
		}

		//float near_plane = 1.0f;
		//float far_plane = 50.0f;
		//glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

		// Assuming lightDirection is normalized
		glm::vec3 lightDirection = direction(); // Your light direction function

		glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

		// Check if the light direction is collinear with the default up vector
		if (glm::abs(glm::dot(lightDirection, upVector)) > 0.999f) {
			upVector = glm::vec3(1.0f, 0.0f, 0.0f); // Choose an alternative up vector
		}

		glm::mat4 lightView = glm::lookAt(-10.0f * lightDirection, glm::vec3(0.0f, 0.0f, 0.0f), upVector);
		

		float near_plane = 0.1f;
		float far_plane = 15.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		m_lightSpaceMatrix = lightProjection * lightView;

		mesh().setTransform(lightView * mesh().transform());
	}
}