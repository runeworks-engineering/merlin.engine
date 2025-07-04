#pragma once
#include "merlin/core/core.h"
#include "merlin/scene/camera.h"
#include "merlin/memory/vao.h"

namespace Merlin {

	enum class ObjectType {
		GENERIC,
		SCENE,
		MODEL,
		MESH, 
		ENVIRONMENT,
		TRANSFORM,
		PARTICLESYSTEM,
	};

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

	class RenderableObject {
	public:
		RenderableObject();
		RenderableObject(std::string name, ObjectType type = ObjectType::GENERIC );

		ObjectType getType() { return m_type; }
		static std::string typeToString(ObjectType);

		virtual void draw(){};

		//Transformation
		void translate(glm::vec3);
		void translate(float x, float y, float z = 0);

		void rotate(glm::vec3);
		void rotate(float angle, glm::vec3 v);
		void setTransform(glm::mat4 t);
		void setPosition(glm::vec3 v);
		void setPosition(float x, float y, float z);
		void scale(glm::vec3 v);
		void scale(float v);
		void alignToDirection(const glm::vec3& direction);//align to direction

		//Getters
		const glm::vec3& position() const;
		const glm::quat& rotation() const;
		const glm::vec3& scale() const;
		inline const glm::mat4& transform() const { return m_transform; }
		glm::mat4& globalTransform();
		inline const std::string name() const { return m_name; }
		inline void rename(std::string n) { m_name = n; };

		inline bool castShadow() const { return m_castShadow; }
		void castShadow(bool state);

		//Hierachy
		void addChild(const shared<RenderableObject>& child);
		void removeChild(shared<RenderableObject> child);
		void setParent(RenderableObject* parent);

		void show();
		void hide();
		inline bool isHidden() const { return m_hidden; }

		bool hasParent() const;
		bool hasChildren() const;

		inline bool isWireFrame() const { return m_wireframe; }
		virtual void enableWireFrameMode();
		virtual void disableWireFrameMode();

		inline const bool useVertexColors() const { return use_vertex_color; }
		inline const bool useFlatShading() const { return use_flat_shading; }
		inline const bool useSmoothShading() const { return !use_flat_shading; }
		inline const bool useNormalMap() const { return use_normal_map; }

		void useVertexColors(bool value);
		void useFlatShading(bool value);
		void useSmoothShading(bool value);
		void useNormalMap(bool value);

		const std::list<shared<RenderableObject>>& children() const;
		shared<RenderableObject> getChild(std::string name);
		RenderableObject* parent();

		void onRenderMenu();

		RenderMode renderMode();
		void setRenderMode(RenderMode mode);

	protected:
		static int nextID;
		bool m_castShadow = true;
		int m_ID;

		RenderMode m_renderMode = RenderMode::LIT;

		bool use_vertex_color = false;
		bool use_normal_map = true;
		bool use_flat_shading = false;

		bool m_wireframe = false;
		bool m_hidden = false;
		std::string m_name;
		glm::mat4 m_transform;

		RenderableObject* m_parent;
		ObjectType m_type;

		std::list<shared<RenderableObject>> m_children;
	};


}
