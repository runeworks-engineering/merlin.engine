#include "pch.h"
#include "merlin/core/core.h"
#include "merlin/graphics/renderableObject.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Merlin {
	
	int RenderableObject::nextID = 0;

	std::string RenderableObject::typeToString(ObjectType ob) {

		switch (ob){
		case Merlin::ObjectType::SCENE:
			return "Scene";
			break;
		case Merlin::ObjectType::MODEL:
			return "Model";
			break;
		case Merlin::ObjectType::MESH:
			return "Mesh";
			break;
		case Merlin::ObjectType::TRANSFORM:
			return "Transform";
			break;
		case Merlin::ObjectType::PARTICLESYSTEM:
			return "ParticleSystem";
			break;
		default:
			return "GenericObject";
			break;
		}
	}

	RenderableObject::RenderableObject() : m_type(ObjectType::GENERIC), m_parent(nullptr), m_transform(glm::mat4(1)) {
		m_ID = nextID++;
		m_name = typeToString(m_type) + std::to_string(m_ID);
	}

	RenderableObject::RenderableObject(std::string name, ObjectType type) : m_type(type), m_parent(nullptr), m_transform(glm::mat4(1)) {
		m_ID = nextID++;
		m_name = name + "_" + std::to_string(m_ID);
	}

	shared<RenderableObject> RenderableObject::getChild(std::string name) {
		for (auto child : m_children) {
			if (child->name() == name) return child;
		}
		return nullptr;
	}

	void RenderableObject::enableWireFrameMode(){
		m_wireframe = true;
		for (auto child : m_children) {
			child->enableWireFrameMode();
		}
	}

	RenderMode RenderableObject::renderMode(){	
		return m_renderMode;
	}

	void RenderableObject::setRenderMode(RenderMode mode) {
		m_renderMode = mode;
		for (auto child : m_children) {
			child->setRenderMode(mode);
		}
	}

	void RenderableObject::disableWireFrameMode(){
		m_wireframe = false;
		for (auto child : m_children) {
			child->disableWireFrameMode();
		}
	}

	void RenderableObject::addChild(const shared<RenderableObject>& child) {
		if (child != nullptr) {
			m_children.push_back(child);
			child->setParent(this);
		}
		else Console::warn("SceneNode") << "Empty node child ignored" << Console::endl;
	}
	void RenderableObject::removeChild(shared<RenderableObject> child) {
		m_children.remove(child);
	}

	void RenderableObject::setParent(RenderableObject* parent) {
		m_parent = parent;
	}

	bool RenderableObject::hasParent() const {
		return m_parent != nullptr;
	}

	bool RenderableObject::hasChildren() const {
		return m_children.size();
	}


	std::list<shared<RenderableObject>>& RenderableObject::children() {
		return m_children;
	}

	RenderableObject* RenderableObject::parent() {
		return m_parent;
	}

	void RenderableObject::onRenderMenu(){
		ImGui::Text((m_name + std::string(" properties")).c_str());
		ImGui::Text("ID: %s", std::to_string(m_ID).c_str());
		
		ImGui::Checkbox("Hidden", &m_hidden);
		ImGui::Checkbox("Wireframe", &m_wireframe);
		ImGui::Checkbox("Cast shadows", &m_castShadow);
		ImGui::Checkbox("Use Vertex Color", &use_vertex_color);
		ImGui::Checkbox("Use Normal Map", &use_normal_map);
		ImGui::Checkbox("Use Flat shading", &use_flat_shading);

		enum class RenderMode {
			UNLIT,
			LIT,
			NORMALS,
			DEPTH,
			POSITION,
			TEXCOORDS,
			SHADOW,
			DEFAULT
		};

		int renderMode = int(m_renderMode);
		static const char* options[] = { "Unlit", "Lit", "Normals", "Depth", "Position", "Texture Coordinate", "Shadows"};
		if (ImGui::ListBox("Colored field", &renderMode, options, 7)) {
			m_renderMode = Merlin::RenderMode(renderMode);
		}
	}


	void RenderableObject::translate(glm::vec3 v) {
		m_transform = glm::translate(m_transform, v);
	}

	void RenderableObject::scale(glm::vec3 v) {
		m_transform = glm::scale(m_transform, v);
	}

	void RenderableObject::scale(float v) {
		m_transform = glm::scale(m_transform, glm::vec3(v));
	}

	void RenderableObject::alignToDirection(const glm::vec3& targetDirection){

		glm::vec3 forward = glm::vec3(1, 0, 0);  // The arrow's original direction (X+)
		glm::vec3 direction = glm::normalize(targetDirection);  // Ensure the direction is normalized

		if (glm::length(direction) < 1e-6f) {
			return; // Avoid division by zero
		}

		// Compute rotation axis (perpendicular to both vectors)
		glm::vec3 rotationAxis = glm::cross(forward, direction);
		float dot = glm::dot(forward, direction);

		if (glm::length(rotationAxis) < 1e-6f) {
			// If vectors are nearly parallel (dot is close to ±1), handle separately
			if (dot > 0.0f) return;  // Already aligned, do nothing
			else rotationAxis = glm::vec3(0, 1, 0); // 180-degree rotation, arbitrary axis
		}

		// Compute the rotation quaternion
		float angle = acos(glm::clamp(dot, -1.0f, 1.0f)); // Ensure within valid range
		glm::quat rotationQuat = glm::angleAxis(angle, glm::normalize(rotationAxis));

		// Convert quaternion to rotation matrix
		glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);
		m_transform = rotationMatrix * m_transform;

		
	}

	void RenderableObject::setPosition(glm::vec3 v) {
		m_transform = glm::translate(m_transform, v - position());
	}

	void RenderableObject::rotate(glm::vec3 v) {
		m_transform = glm::rotate(m_transform, v.x, glm::vec3(1, 0, 0));
		m_transform = glm::rotate(m_transform, v.y, glm::vec3(0, 1, 0));
		m_transform = glm::rotate(m_transform, v.z, glm::vec3(0, 0, 1));
	}

	void RenderableObject::rotate(float angle, glm::vec3 v) {
		m_transform = glm::rotate(m_transform, angle, v);
	}

	void RenderableObject::setTransform(glm::mat4 t) {
		m_transform = t;
	}

	const glm::vec3& RenderableObject::position() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		return translation;
	}

	const glm::quat& RenderableObject::rotation() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		return rotation;
	}

	const glm::vec3& RenderableObject::scale() const {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(m_transform, scale, rotation, translation, skew, perspective);
		return scale;
	}

	glm::mat4& RenderableObject::globalTransform() {
		//loop to the scene graph to get global transform
		glm::mat4 globalTransform = glm::mat4(1.0f); // Initialize with identity matrix
		std::vector<glm::mat4> nodeStack;

		RenderableObject* node = this;
		int it = 0;

		while (node && it < 50) {// Push all nodes to the stack
			nodeStack.push_back(node->transform());
			node = node->parent();
		}

		// Multiply matrices in the correct order (from root to leaf)
		for (auto it = nodeStack.rbegin(); it != nodeStack.rend(); ++it) {
			globalTransform = globalTransform * (*it);
		}
		return globalTransform;
	}


}